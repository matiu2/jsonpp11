/// Parses an array
#pragma once

#include "status.hpp"
#include "utils.hpp"

namespace json {

/// Convenience function to read an object.
/// @onVal will be read when the stream is ready to read a value. The function
///        needs to consume that value from the stream.
template <typename Status>
void readArray(Status &status, std::function<void(Token)> onVal) {
  auto& p = status.p;
  auto& pe = status.pe;
  while (p != pe) {
    // Read the next value
    auto acceptable = valueTokens();
    acceptable.insert(ARRAY_END);
    Token token = require(acceptable, status);
    if (token == ARRAY_END)
      return; 
#ifndef NDEBUG
    auto b4 = p;
    onVal(token);
    assert(b4 != p); // onVal must consume one value
#else
    onVal(token);
#endif
    // Read the comma or the end of the object
    token = require({COMMA, ARRAY_END}, status);
    if (token == ARRAY_END)
      break;
  }
}


}
