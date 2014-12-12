/// Here are all the unicode conversion functions that we use to read/write utf-8 utf-16 and utf-32
/// utf-32 is treated as the gold standard, so 'to16' means from utf-12 to utf-16.
#pragma once

#include "utils.hpp"

#include <stdexcept>
#include <cassert>


namespace json {

/// Returns the number of characters needed to record the unicode character 'u'
/// to a medium of 'T'
template <typename T> int getNumChars(char32_t u) {
  switch (sizeof(T)) {
  case 1: {
    int numBytes = 1;
    char32_t checker = 0x7f;
    if (u > checker) {
      checker <<= 4;
      checker |= 0xf;
      numBytes += 1;
      while (u > checker) {
        checker <<= 5;
        checker |= 0x1f;
        ++numBytes;
      }
    }
    return numBytes;
  }
  case 2:
    return ((u <= 0xD7FF) || (u >= 0xE000)) ? 1 : 2;
  case 4:
    return 1;
  };
}

/// Thrown for unicode encoding/decoding errors
struct UnicodeError : std::runtime_error {
  UnicodeError(const char* msg) : std::runtime_error(msg) {}
};

/**
* @brief Converts from utf-8 to utf-32; takes two iterators.
*
* @tparam In The input iterator type (should provide 8 bit chars).
* @tparam Out The output iterator type (should write 32 bit chars).
* @tparam InTraits The input iterator traits.
* @param in The input iterator (should provide 8 bit chars)
* @param out The output iterator (we write 32 bit chars)
*/
template <typename In, typename Out,
          typename InTraits=std::iterator_traits<In>>
inline void from8(In in, Out out) {
  static_assert(is_input_iterator<In>(), "We read in UTF-8 and output UTF-32");
  static_assert(is_output_iterator<Out>(), "We read in UTF-8 and output UTF-32");
  static_assert(sizeof(typename InTraits::value_type) == 1, "Expected the input to be 8 bits at a time");

  /*  Nibble table
   *  0000 0     1000 8
   *  0001 1     1001 9
   *  0010 2     1010 A
   *  0011 3     1011 B
   *  0100 4     1100 C
   *  0101 5     1101 D
   *  0110 6     1110 E
   *  0111 7     1111 F
   */

  /*
   * Enocding table
     Num Bytes  UCS-4 range (hex.)  UTF-8 octet sequence (binary)
     1          0000 0000-0000 007F 0xxxxxxx
     2          0000 0080-0000 07FF 110xxxxx 10xxxxxx
     3          0000 0800-0000 FFFF 1110xxxx 10xxxxxx 10xxxxxx

     4          0001 0000-001F FFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     5          0020 0000-03FF FFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
     6          0400 0000-7FFF FFFF 1111110x 10xxxxxx ... 10xxxxxx
  */

  // Read the first byte
  unsigned char byte = *(in++);

  // Simplest most common case - one byte encoding
  if ((byte & 0x80) == 0) {
    *out = byte;
    return;
  }

  /* If it's a 2 byte encoding:
   * 'byte' will have the encoding: 110x xxxx
   * byte format = 110x xxxx
   * mask        = 1110 0000 = 0xE0;
   * expected    = 1100 0000 = 0xC0;
   * not_mask    = 1100 0000 = 0xC0;
   * if 110x xxxx & 0011 1111 == 0001 1111 then it's a two byte encoding
   * if p         & 0x3F      == 0xC0      then it's a two byte encoding
   *
   * Then we start the cycle again having the mask be 0x1F (0001 1111)
   * If it's a 3 byte encoding:
   * Byte format = 1110 xxxx
   * mask        = 0001 1111
   */

  unsigned char mask = 0xE0;
  char32_t result = 0;
  int bytesToRead = 1; // Number of bytes left to read .. will be filled in once
                       // we've read the first byte

  // Read the first byte
  while (bytesToRead < 6) {
    unsigned char tmp = byte & mask;
    if (tmp == (unsigned char)(mask << 1)) {
      // Get the actual encoded value
      byte &= ~mask;  // 110x xxxx & 0011 1111 = 000x xxxx
      break;
    }
    mask = (mask >> 1) | 0x80; // 1100 000 => 1110 0000
    ++bytesToRead;
  }

  // If 'byte' has 7 1s at the top, it's not utf-8 (1111 1110)
  if ((byte & 0xFE) == 0xFE)
    throw UnicodeError("Bad utf-8 first char");

  result = byte;

  // Now get the rest of the bytes
  for (;bytesToRead;--bytesToRead) {
    byte = *(in++);
    // Each subsequent char must have the format 10xx xxxx
    // 10xx xxxx & 1100 0000 must equal 1000 0000
    if ((byte & 0xC0) != 0x80)
      throw UnicodeError("Bad utf-8 char");
    result = (result << 6) | (byte & 0x3F); // Read in the 6 bytes of data
  }

  *(out++) = result;
}

/**
* @brief Converts from utf-16 to utf-32; takes two iterators.
*
* @tparam In The input iterator type (should provide 16 bit chars).
* @tparam Out The output iterator type (should write 32 bit chars).
* @tparam InTraits The input iterator traits.
* @param in The input iterator (should provide 16 bit chars)
* @param out The output iterator (we write 32 bit chars)
*/
template <typename In, typename Out, typename InTraits = std::iterator_traits<In>,
          typename OutTraits = std::iterator_traits<Out>>
inline void from16(In in, Out out) {
  static_assert(is_input_iterator<In>(), "We read in UTF-16 and output UTF-32");
  static_assert(sizeof(typename InTraits::value_type) == 2, "Expected the input to be 16 bits at a time");

  // Is this in one of the first plane ?
  if ((*in <= 0xD7FF) || (*in >= 0xE000)) {
    *out = static_cast<char32_t>(*in);
  } else {
    char16_t top = *(in++);
    char16_t bottom = *in;
    top -= 0xD800;
    bottom -= 0xDC00;
    char32_t result = top << 10;
    result |= bottom;
    result += 0x010000;
    // Output
    *out = result;
  }
  ++in;
}

/**
* @brief Converts from utf-32 to utf-8; takes two iterators.
*
* @tparam In The input iterator type (should provide 32 bit chars).
* @tparam Out The output iterator type (should write 8 bit chars).
* @tparam InTraits The input iterator traits.
* @param in The input iterator (should provide 32 bit chars)
* @param out The output iterator (we write 8 bit chars)
*/
template <typename In, typename Out,
          typename InTraits=std::iterator_traits<In>>
inline int to8(In in, Out out) {
  static_assert(is_input_iterator<In>(), "We read in UTF-32 and output UTF-8");
  static_assert(sizeof(typename InTraits::value_type) == 4, "Expected the input to be 32 bits wide");

  using Char = unsigned char;

  /*
     UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     0000 0000-0000 007F   0xxxxxxx
     0000 0080-0000 07FF   110xxxxx 10xxxxxx
     0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

     0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
     0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
  */

  // Get the unicode char
  char32_t u = *in;

  // Get the number of bytes needed to encode a unicode character in utf8
  int numChars = getNumChars<unsigned char>(u);
  assert(numChars <= 6);
  if (numChars > 6)
    throw UnicodeError("UTF-8 can only encode 6 bytes");

  Char stack[6]; // Maximum of 6 bytes for any utf8 encoding. Stack so we can
                 // reverse the order on the way out.
  Char *up = stack; // We need to reverse the output order
  if (numChars == 1) {
    *(up++) = static_cast<Char>(u);
  } else {
    for (int i = 1; i < numChars; ++i) {
      Char byte = u & 0x3f; // Only encoding 6 bits right now
      byte |= 0x80;         // Make sure the high bit is set
      *(up++) = byte;
      u >>= 6;
    }
    // The last byte is special
    const Char mask = 0x3f >> (numChars - 2);
    Char byte = u & mask;
    Char top = 0xc0;
    for (int i = 2; i < numChars; ++i) {
      top >>= 1;
      top |= 0x80;
    }
    byte |= top;
    *(up++) = byte;
  }
  // Unravel the stack
  while (up != stack)
    *(out++) = *(--up);
  return numChars;
}

/**
* @brief Converts from utf-32 to utf-16; takes two iterators.
*
* @tparam In The input iterator type (should provide 32 bit chars).
* @tparam Out The output iterator type (should write 16 bit chars).
* @tparam InTraits The input iterator traits.
* @param in The input iterator (should provide 32 bit chars)
* @param out The output iterator (we write 16 bit chars)
*/
template <typename In, typename Out,
          typename InTraits=std::iterator_traits<In>>
inline int to16(In in, Out out) {
  static_assert(is_input_iterator<In>(), "We read in UTF-32 and output UTF-16");
  static_assert(sizeof(typename InTraits::value_type) == 4, "Expected the input to be 32 bits wide");

  // Is this in one of the first plane ?
  int bytesWritten = 1;
  if ((*in <= 0xD7FF) || (*in >= 0xE000)) {
    *out = static_cast<char16_t>(*in);
  } else {
    char32_t chr = *in - 0x10000;
    char16_t top = chr >> 10;      // Top 10 bits
    char16_t bottom = chr & 0x3ff; // Bottom 10 bits
    top += 0xD800;
    bottom += 0xDC00;
    // Output
    *(out++) = top;
    *out = bottom;
    bytesWritten = 2;
  }
  ++in;
  return bytesWritten;
}

/// Repeatedly applies one of the above functions to iterators
template<typename In, typename Out>
inline void transform(In begin, In end, Out out, std::function<void(In, Out)> converter) {
  while (begin != end)
    converter(begin, out);
}

/// Repeatedly transforms utf8 encoded text into another type unicode
template<typename In, typename Out>
inline void transformFrom8(In begin, In end, Out out) {
  while (begin != end)
    from8(begin, out);
}


}
