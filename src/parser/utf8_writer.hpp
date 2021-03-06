#pragma once
/// Funtions to convert wide characters to a utf8 encoded character sequence

#include <cassert>

namespace json {

/**
 * Returns the number of bytes needed to encode a unicode character in utf8
 */
inline int getNumBytes(long long c) {
  int result = 1;
  long long checker = 0x7f;
  if (c <= checker)
    return result;
  checker <<= 4;
  checker |= 0xf;
  result += 1;
  if (c <= checker)
    return result;
  while (c > checker) {
    checker <<= 5;
    checker |= 0x1f;
    ++result;
  }
  return result;
}

/// After collecting all the unicode integers, we generate the final output char
template <typename Iterator>
inline Iterator utf8encode(long long u, Iterator p) {
  /*
     UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     0000 0000-0000 007F   0xxxxxxx
     0000 0080-0000 07FF   110xxxxx 10xxxxxx
     0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

     0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
     0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
  */
  int numBytes =
      getNumBytes(u); // number of bytes needed for utf-8 encoding
  char stack[4]; // Maximum of 4 bytes for any utf8 encoding
  assert(numBytes <= 4);
  char *out = stack; // We need to reverse the output order
  if (numBytes == 1) {
    *(out++) = static_cast<char>(u);
  } else {
    for (int i = 1; i < numBytes; ++i) {
      char byte = u & 0x3f; // Only encoding 6 bits right now
      byte |= 0x80;         // Make sure the high bit is set
      *(out++) = byte;
      u >>= 6;
    }
    // The last byte is special
    const char mask = 0x3f >> (numBytes - 2);
    char byte = u & mask;
    char top = 0xc0;
    for (int i = 2; i < numBytes; ++i) {
      top >>= 1;
      top |= 0x80;
    }
    byte |= top;
    *(out++) = byte;
  }
  // Unravel the stack
  while (out != stack)
    *(p++) = *(--out);
  return p;
}
}
