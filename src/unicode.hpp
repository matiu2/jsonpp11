/// Here are all the unicode conversion functions that we use to read/write utf-8 utf-16 and utf-32
/// utf-32 is treated as the gold standard, so 'to16' means from utf-12 to utf-16.
#pragma once

#include "utils.hpp"

namespace json {

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
          typename InTraits=iterator_traits<In>>
inline void from8(In , Out ) {
  static_assert(is_input_iterator<In>(), "We read in UTF-8 and output UTF-32");
  static_assert(is_output_iterator<Out>(), "We read in UTF-8 and output UTF-32");
  static_assert(sizeof(typename InTraits::value_type) == 1, "Expected the input to be 8 bits at a time");

  /*
     UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     0000 0000-0000 007F   0xxxxxxx
     0000 0080-0000 07FF   110xxxxx 10xxxxxx
     0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

     0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
     0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
  */

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
template <typename In, typename Out, typename InTraits = iterator_traits<In>,
          typename OutTraits = iterator_traits<Out>>
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
          typename InTraits=iterator_traits<In>>
inline void to8(In , Out ) {
  static_assert(is_input_iterator<In>(), "We read in UTF-32 and output UTF-8");
  static_assert(is_output_iterator<Out>(), "We read in UTF-32 and output UTF-8");
  static_assert(sizeof(typename InTraits::value_type) == 4, "Expected the input to be 32 bits wide");
  static_assert(is_assignable<remove_pointer<Out>, char>(),
                "out should be an output iterator that lets us write char");

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
          typename InTraits=iterator_traits<In>>
inline void to16(In in, Out out) {
  static_assert(is_input_iterator<In>(), "We read in UTF-32 and output UTF-16");
  static_assert(is_output_iterator<Out>(), "We read in UTF-32 and output UTF-16");
  static_assert(sizeof(typename InTraits::value_type) == 4, "Expected the input to be 32 bits wide");
  static_assert(is_assignable<remove_pointer<Out>, char16_t>(),
                "out should be an output iterator that lets us write char16_t");

  // Is this in one of the first plane ?
  if ((*in <= 0xD7FF) || (*in >= 0xE000)) {
    *(out++) << static_cast<char16_t>(*in);
  } else {
    char32_t chr = *in - 0x10000;
    char16_t top = chr >> 10;      // Top 10 bits
    char16_t bottom = chr & 0x3ff; // Bottom 10 bits
    top += 0xD800;
    bottom += 0xDC00;
    // Output
    *(out++) << top;
    *(out++) << bottom;
  }
  ++in;
}

}
