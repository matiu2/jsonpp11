#pragma once

#include "error.hpp"
#include "utils.hpp"
#include "utf8_writer.hpp"

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
template <typename Iterator>
inline void
parseString(Iterator p, Iterator pe,
            std::function<void(Iterator, Iterator)>
                recordUnchangedChars, /// Takes the begin and end iterators
                                      /// of a block of unchanged chars
            std::function<void(char)> recordChar,
            std::function<void(wchar_t)> recordUnicode,
            ErrorThrower<Iterator> onError = throwError<Iterator>) {
  /// Handle the 4 digits of a unicode char
  auto readUnicode = [&]() {
    wchar_t u = 0;
    int uniCharNibbles = 0;
    while (p != pe) {
      if (uniCharNibbles == sizeof(wchar_t)*2) {
        std::stringstream msg;
        msg << "Max unicode char is " << sizeof(wchar_t) << "  bytes";
        onError(msg.str(), p);
      }
      char ch = *p;
      if ((ch >= '0') && (ch <= '9')) {
        u <<= 4;
        u += ch - '0';
      } else if ((ch >= 'a') && (ch <= 'f')) {
        u <<= 4;
        u += ch - 'a' + 0x0A;
      } else if ((ch >= 'A') && (ch <= 'F')) {
        u <<= 4;
        u += ch - 'A' + 0x0A;
      } else
        break;
      ++p;
      ++uniCharNibbles;
    }
    // We didn't get any unicode digits
    if (uniCharNibbles == 0) {
      onError("\\u with no hex after it", p);
    } else {
      return u;
    }
    assert("Code should never reach here, onError should throw above");
    return u;
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
      recordUnicode(readUnicode());
      break;
    default:
      return false;
    };
    return true;
  };

  // Main outter string parsing loop
  Iterator unchangedCharsStart = p;
  while (p != pe) {
    switch (*p) {
    case '"':
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
  size_t size() const {
    if (is_random_access_iterator<Iterator>())
      return _end - _begin;
    else {
      size_t result = 0;
      Iterator i = _begin;
      while (i != _end) {
        ++i;
        ++result;
      }
      return result;
    }
  }
  template <typename T>
  bool operator ==(const T& other) const {
    return equal(cbegin(), cend(), other.cbegin(), other.cend());
  }
};


/**
 * @brief Return the size of a raw JSON encoded input string
 *
 * eg. input: \u209\x\"\n abc"
 * It ignore's the final quote (meaning end of string) and returns 15. Just
 *counting each char once
 *
 * @tparam Iterator input iterator type
 * @tparam Traits iterator_traits
 * @param p Start of JSON input, just after the first '"'
 * @param pe One past the end of the JSON input
 *
 * @returns the length of the raw JSON string
 */
template <typename Iterator, typename Traits = iterator_traits<Iterator>>
inline size_t getRawStringLength(Iterator p, Iterator pe) {
  static_assert(is_input_iterator<Traits>(),
                "We need to be able to read from the input");
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
 * @tparam Iterator input iterator type
 * @tparam Traits iterator_traits
 * @param p Start of JSON input, just after the first '"'
 * @param pe One past the end of the JSON input
 *
 * @returns The number of bytes needed to encode this json string
 */
template <typename Iterator, typename Traits = iterator_traits<Iterator>>
inline size_t getDecodedStringLength(Iterator p, Iterator pe) {
  static_assert(is_input_iterator<Traits>(),
                "We need to be able to read from the input");
  size_t result = 0;
  auto recordUnchangedChars = [&](Iterator ucBegin, Iterator ucEnd) {
    if (is_random_access_iterator<Iterator>)
      result += ucEnd - ucBegin;
    else
      while (ucBegin != ucEnd)
        ++result;
  };
  auto recordChar = [&](char c) { ++result; };
  auto recordUnicode = [&](wchar_t u) { result += getNumBytes(u); };
  parseString<Iterator>(p, pe, recordUnchangedChars, recordChar, recordUnicode);
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
 * @tparam Iterator input iterator type
 * @tparam Traits iterator_traits
 * @param p Start of JSON input, just after the first '"'
 * @param pe One past the end of the JSON input
 *
 * @returns a pair of iterators to the beginning and end of the decoded string
 */
template <typename Iterator, typename Traits = iterator_traits<Iterator>>
inline string_reference<Iterator> decodeStringInPlace(Iterator p, Iterator pe) {
  static_assert(is_input_iterator<Traits>(),
                "We need to be able to read from the input");
  static_assert(is_copy_assignable<remove_pointer<Iterator>>(),
                "We need to be able to write to the input too");
  Iterator begin = p, end = p;
  bool hadChangedChars = false;
  // Just increments the output string's length
  auto recordUnchangedChars = [&](Iterator ucBegin, Iterator ucEnd) {
    if (!hadChangedChars) {
      // If we haven't had any changed chars, just increment our output position
      if (is_random_access_iterator<Iterator>())
        end += ucEnd - ucBegin;
      else
        while (ucBegin != ucEnd) {
          ++ucBegin;
          ++end;
        }
    } else {
      // Overwrite the output. All JSON converstions are shorter than the raw
      // json.
      // So the output will easily be contained within the original json
      // string's size
      end = std::copy(ucBegin, ucEnd, end);
    }
  };
  // Just advance the end pointer
  auto recordChar = [&](char c) { *(end++) = c; };
  // UTF-8 encode a unicode char
  auto recordUnicode = [&](wchar_t u) { end = utf8encode(u, end); };
  parseString<Iterator>(p, pe, recordUnchangedChars, recordChar, recordUnicode);
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
 * @tparam Iterator input iterator type
 * @tparam Traits iterator_traits
 * @tparam OutputIterator output iterator type
 * @tparam OutputTraits iterator_traits
 * @param p Start of JSON input, just after the first '"'
 * @param pe One past the end of the JSON input
 * @param out the output iterator, usually a back_inserter to a std::string
 *
 * @returns the number of bytes copied
 */
template <typename Iterator, typename OutputIterator,
          typename Traits = iterator_traits<Iterator>,
          typename OutputTraits = iterator_traits<OutputIterator>>
size_t copyRawString(Iterator p, Iterator pe, OutputIterator out) {
  static_assert(is_input_iterator<Traits>(),
                "We need to be able to read from the input");
  static_assert(is_output_iterator<OutputTraits>(),
                "We need to be able to write to the output");
  size_t result = getRawStringLength(p, pe); // end is written to
  std::copy_n(p, result, out);
  return result;
}

/**
 * @brief Decodes a json string to an output iterator
 *
 * It's advisable to use getDecodedStringLength() or getRawStringLength() to
 * allocate the memory needed in one go, as allocating on the fly can cause
 * unnecessary memory copying.
 *
 * @tparam Iterator input iterator type
 * @tparam Traits iterator_traits
 * @tparam OutputIterator output iterator type
 * @tparam OutputTraits iterator_traits
 * @param p Start of JSON input, just after the first '"'
 * @param pe One past the end of the JSON input
 * @param out the output iterator, usually a back_inserter to a std::string
*/
template <typename Iterator, typename OutputIterator,
          typename Traits = iterator_traits<Iterator>,
          typename OutputTraits = iterator_traits<OutputIterator>>
inline void decodeString(Iterator p, Iterator pe, OutputIterator out) {
  static_assert(is_input_iterator<Traits>(),
                "We need to be able to read from the input");
  static_assert(is_output_iterator<OutputTraits>(),
                "We need to be able to write to the output iterator");
  bool hadChangedChars = false;
  // Just increments the output string's length
  auto recordUnchangedChars =
      [&](Iterator ucBegin, Iterator ucEnd) { std::copy(ucBegin, ucEnd, out); };
  // Just advance the end pointer
  auto recordChar = [&](char c) { *(out++) = c; };
  // UTF-8 encode a unicode char
  auto recordUnicode = [&](wchar_t u) { out = utf8encode(u, out); };
  parseString<Iterator>(p, pe, recordUnchangedChars, recordChar, recordUnicode);
}

}
