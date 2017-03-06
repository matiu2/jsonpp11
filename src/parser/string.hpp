#pragma once

#include "error.hpp"
#include "../utils.hpp"
#include "../unicode.hpp"
#include "status.hpp"
#include "utf8_writer.hpp"

#include <string>
#include <algorithm>
#include <iterator>
#include <cassert>

namespace json {

/// When we come across a block of normal chars, return the char just past the
/// end of it
template <typename Iterator>
inline Iterator findEndOfUnchangedCharBlock(Iterator p, Iterator pe) {
  while (p != pe) {
    switch (*p) {
    case '\\':
    case '"':
      return p;
    default:
      ++p;
    };
  }
  return p;
}

/// Most basic string parser. Calls back functions for each token/block that it
/// finds.
template <typename Status,
          typename f1 = std::function<
              void(typename Status::iterator, typename Status::iterator)>,
          typename f2 =
              std::function<void(typename Status::iterator_traits::value_type)>>
inline void
parseString(Status &status,
            f1 recordUnchangedChars, /// Takes the begin and end iterators of a
                                     /// block of unchanged chars
            f2 recordChar, std::function<void(char32_t)> recordUnicode) {

  BOOST_HANA_CONSTANT_CHECK(is_valid_status(status));
  BOOST_HANA_CONSTANT_ASSERT(is_forward_iterator(status.p));

  using Char = typename Status::iterator_traits::value_type;
  auto& p = status.p;
  const auto& pe = status.pe;

  /// Handle the 4 digits of a unicode char
  std::function<char32_t(Status &)> readUnicode =
      [&readUnicode](Status &s) -> char32_t {
    char16_t u[2] = {0,0};
    int uniCharNibbles = 0;
    auto& p = s.p;
    auto& pe = s.pe;
    while (p != pe) {
      if (uniCharNibbles == 8) {
        std::stringstream msg;
        msg << "A unicode character must be of the format \u0000 where 0000 "
               "are 4 hexadecimal characters";
        s.onError(msg.str());
      }
      Char ch = *p;
      if ((ch >= '0') && (ch <= '9')) {
        *u <<= 4;
        *u += ch - '0';
      } else if ((ch >= 'a') && (ch <= 'f')) {
        *u <<= 4;
        *u += ch - 'a' + 0x0A;
      } else if ((ch >= 'A') && (ch <= 'F')) {
        *u <<= 4;
        *u += ch - 'A' + 0x0A;
      } else
        break;
      ++p;
      ++uniCharNibbles;
    }
    // See how many hex characters we got
    switch (uniCharNibbles) {
    case 0:
      s.onError("\\u with no hex after it");
    case 1:
    case 2:
    case 3:
      s.onError("\\u needs 4 hex chars after it");
    case 4:
      // See if we're followed by another \u
      auto peeker = s;
      if (checkStaticString(peeker, R"(\u)")) {
        u[1] = readUnicode(peeker);
        char32_t result;
        // Decode the two utf chars
        from16(u, &result);
        s = peeker;
        return result;
      }
      return *u;
    }
    assert("Code should never reach here, onError should throw above");
    return *u;
  };
  /// Handle the escaped character after the backslash
  /// @return true if we handled it; false if it turned out to be just an normal
  /// char (eg '\\')
  auto handleEscape = [&]() {
    switch (*++p) {
    case 'b':
      recordChar('\b');
      ++p;
      break;
    case 'f':
      recordChar('\f');
      ++p;
      break;
    case 'n':
      recordChar('\n');
      ++p;
      break;
    case 'r':
      recordChar('\r');
      ++p;
      break;
    case 't':
      recordChar('\t');
      ++p;
      break;
    case 'u':
      ++p; // Skip over the 'u'
      recordUnicode(readUnicode(status));
      // Skip ahead to see if it's two 16 b
      break;
    default:
      return false;
    };
    return true;
  };

  // Main outer string parsing loop
  auto unchangedCharsStart = p;
  while (p != pe) {
    switch (*p) {
    case '"':
      ++p;
      return;
    case '\\':
      if (handleEscape())
        break;
    // If the escaped char was just a normal char, it will be the start of a
    // chain of normal chars, so don't break out of the switch statement here;
    // continue on to handle the chain
    default: {
      unchangedCharsStart = p;
      p = findEndOfUnchangedCharBlock(p, pe);
      recordUnchangedChars(unchangedCharsStart, p);
    }
    }
  }
}

/**
* @brief Like a string, but just contains two iterators that reference a larger
*text
*
* @tparam Iterator Iterator type
*/
template <typename Iterator> struct string_reference {
  using iterator = Iterator;
  using const_iterator = Iterator;

  Iterator _begin;
  Iterator _end;

  inline const_iterator cbegin() const { return _begin; }
  inline const_iterator cend() const { return _end; }
  inline iterator begin() { return _begin; }
  inline iterator end() { return _end; }
  inline iterator begin() const { return _begin; }
  inline iterator end() const { return _end; }
  inline size_t size() const { return std::distance(_begin, _end); }
  template <typename T> bool operator==(const T &other) const {
    return equal(cbegin(), cend(), other.cbegin(), other.cend());
  }
  operator std::string() const {
    std::string result;
    result.reserve(size());
    std::copy(cbegin(), cend(), std::back_inserter(result));
    return result;
  }
};

/**
 * @brief Return the size of a raw JSON encoded input string
 *
 * eg. input: \u209\x\"\n abc"
 * It ignore's the final quote (meaning end of string) and returns 15. Just
 *counting each char once
 *
 * @tparam Status Some kind of parser.hpp Status template instantiation
 * @param Status A reference to a valid parser status instance
 *
 * @returns the length of the raw JSON string
 */
template <typename Status>
inline size_t getRawStringLength(Status status) {

  BOOST_HANA_CONSTANT_CHECK_MESSAGE(is_valid_status(status),
                                    "The status object must have "
                                    "iterators p, pe, and an error "
                                    "thrower function; onError");

  auto& p = status.p;
  const auto& pe = status.pe;

  size_t result = 0;
  while (p != pe) {
    switch (*p) {
    case '"':
      return result;
    case '\\':
      ++p; // Skip over the next char
      ++result;
    }
    ++p;
    ++result;
  }
  return result;
}

/**
 * @brief Returns the length of what a JSON string would be if it were encoded
 *
 * Takes a raw JSON string, and runs through it calculating how many bytes it'll
 * need, should you ask it to decode it in the future. This is more efficient
 * (mainly in that it doesn't have to allocate memory, which can cause a lot of
 * unneccesary data copying) than actually encoding it, and could be useful for
 * deciding how much memory to allocate before decoding a string on the fly.
 *
 * @tparam Status Some kind of parser.hpp Status template instantiation
 * @param Status A reference to a valid parser status instance
 *
 * @returns The number of bytes needed to encode this json string
 */
template <typename Status>
inline size_t getDecodedStringLength(Status status) {

  using Iterator = typename Status::iterator;

  size_t result = 0;
  auto recordUnchangedChars = [&](Iterator ucBegin, Iterator ucEnd) {
    result += std::distance(ucBegin, ucEnd);
  };
  auto recordChar = [&](char) { ++result; };
  auto recordUnicode = [&](char32_t u) {
    result += getNumChars<typename Status::iterator_traits::value_type>(u);
  };
  parseString(status, recordUnchangedChars, recordChar, recordUnicode);
  return result;
}

/**
 * @brief Decodes a JSON string over the top of itself, overwriting the input
 *
 * This is the most efficient way to parse a string, as no allocation, nor
 * length calculation is needed. It returns a pair of iterators to the beginning
 * of the data and the new end (of the encoded string). We can do this because
 * the decoded string length is always <= the JSON string length.
 *
 * @tparam Status Some kind of parser.hpp Status template instantiation
 * @param Status A reference to a valid parser status instance
 *
 * @returns a pair of iterators to the beginning and end of the decoded string
 */
template <typename Status>
inline string_reference<typename Status::iterator> decodeStringLight(Status &status) {

  BOOST_HANA_CONSTANT_CHECK(is_valid_status(status));
  BOOST_HANA_CONSTANT_CHECK(is_forward_iterator(status.p) &&
                            is_output_iterator(status.p));

  using Iterator = typename Status::iterator;
  using Char = typename Status::iterator_traits::value_type;

  auto &p = status.p;

  Iterator begin = p, end = p;
  bool hadChangedChars = false;
  // Just increments the output string's length
  auto recordUnchangedChars = [&](Iterator ucBegin, Iterator ucEnd) {
    if (!hadChangedChars) {
      // If we haven't had any changed chars, just increment our output position
      boost::hana::if_(is_random_access_iterator(end),
                       [](auto &end, size_t n) { end += n; },
                       [](auto &end, size_t n) {
                         for (size_t i = 0; i < n; ++i)
                           ++end;
                       })(end, std::distance(ucBegin, ucEnd));
    } else {
      // Overwrite the output. All JSON converstions are shorter than the raw
      // json.
      // So the output will easily be contained within the original json
      // string's size
      end = std::copy(ucBegin, ucEnd, end);
    }
  };
  // Just advance the end pointer
  auto recordChar = [&](Char c) {
    *end = c;
    ++end;
  };
  // UTF-8 encode a unicode char
  auto recordUnicode = [&](char32_t u) {
    switch (sizeof(Char)) {
    case 1: {
      int i = to8(&u, end);
      while (i--)
        ++end; // TODO: Find a better way to increase the iterator
      break;
    }
    case 2: {
      int i = to16(&u, end);
      while (i--)
        ++end; // TODO: Find a better way to increase the iterator
      break;
    }
    case 4: {
      *(end++) = u;
      break;
    }
    };
  };
  parseString(status, recordUnchangedChars, recordChar, recordUnicode);
  return {begin, end};
}

/**
 * @brief Copy's a raw JSON string (no decoding) to an output iterator
 *
 * It's advisable to use custom allocators, or to use getRawStringLength() and
 * reserve the amount of memory needed before calling copyRawString(), as using
 *a
 * back_inserter and re-allocating memory as needed can slow things down quite a
 * lot.
 *
 * @tparam Status Some kind of parser.hpp Status template instantiation
 * @param Status A reference to a valid parser status instance
 * @tparam OutputIterator output iterator type
 * @tparam OutputTraits std::iterator_traits
 * @param p Start of JSON input, just after the first '"'
 * @param pe One past the end of the JSON input
 * @param out the output iterator, usually a back_inserter to a std::string
 *
 * @returns the number of bytes copied
 */
template <typename Status, typename OutputIterator,
          typename OutputTraits = std::iterator_traits<OutputIterator>>
size_t copyRawString(Status &status, OutputIterator out) {
  BOOST_HANA_CONSTANT_ASSERT(is_output_iterator(out));
  size_t result = getRawStringLength(status.p, status.pe); // end is written to
  std::copy_n(status.p, result, out);
  return result;
}

/**
 * @brief Decodes a json string to an output iterator
 *
 * It's advisable to use getDecodedStringLength() or getRawStringLength() to
 * allocate the memory needed in one go, as allocating on the fly can cause
 * unnecessary memory copying.
 *
 * @tparam Status Some kind of parser.hpp Status template instantiation
 * @param Status A reference to a valid parser status instance
 * @tparam OutputIterator output iterator type
 * @tparam OutputTraits std::iterator_traits
 * @param p Start of JSON input, just after the first '"'
 * @param pe One past the end of the JSON input
 * @param out the output iterator, usually a back_inserter to a std::string
*/
template <typename Status, typename OutputIterator,
          typename OutputTraits = std::iterator_traits<OutputIterator>>
inline void decodeString(Status &status, OutputIterator out) {

  BOOST_HANA_CONSTANT_ASSERT(is_output_iterator(out));
  BOOST_HANA_CONSTANT_ASSERT(is_valid_status(status));
  BOOST_HANA_CONSTANT_ASSERT(is_forward_iterator(status.p));

  using Iterator = typename Status::iterator;
  using value_type = typename Status::iterator_traits::value_type;

  // Just increments the output string's length
  auto recordUnchangedChars = [&](Iterator ucBegin, Iterator ucEnd) {
    std::copy(ucBegin, ucEnd, out);
  };
  // Just advance the end pointer
  auto recordChar = [&](value_type c) { *(out++) = c; };
  // UTF-8 encode a unicode char
  auto recordUnicode = [&](char32_t u) { out = utf8encode(u, out); };
  parseString(status, recordUnchangedChars, recordChar, recordUnicode);
}

/// Decodes a JSON string
template <typename Status> std::string decodeString(Status &status) {
  BOOST_HANA_CONSTANT_ASSERT(is_valid_status(status));
  BOOST_HANA_CONSTANT_ASSERT(is_forward_iterator(status.p));

  std::string result;
  result.reserve(getDecodedStringLength(status));
  decodeString(status, std::back_inserter(result));
  return result;
}
}
