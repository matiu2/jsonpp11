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
    Token next = getNextOuterToken(status);
    // Read the attribute name or the end of the object
    if (next == ARRAY_END)
      // We're done
      break;
    // Read the next value
    next = expectAnyRealType(status);
    if (next == ARRAY_END)
      return; 
#ifndef NDEBUG
    auto b4 = p;
    onVal(next);
    assert(b4 != p); // onVal must consume one value
#else
    onVal(next);
#endif
    // Read the comma or the end of the object
    next = getNextOuterToken(status);
    if (next == ARRAY_END)
      // We're done
      break;
    if (!expect(COMMA, next, status))
      // We needed a comma to continue without error
      break;
  }
}


}
