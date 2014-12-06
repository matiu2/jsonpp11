#pragma once

#include "outer.hpp"
#include "number.hpp"
#include "string.hpp"
//#include "array.hpp"
#include "../utils.hpp"
#include "utils.hpp"
#include "error.hpp"

#include <stdexcept>

namespace json {

/// Convenience function to read an object.
/// @onAttribute will be called when an attribute name is read
/// @onVal will be read when the stream is ready to read a value. The function
///        needs to consume that value from the stream.
template <typename Status>
void readObject(Status &status, std::function<void(std::string &&)> onAttribute,
                std::function<void(Token)> onVal) {
  auto& p = status.p;
  auto& pe = status.pe;
  while (p != pe) {
    // We need an attribute, or the end of the object
    Token token = require({OBJECT_END, string}, status);
    if (token == OBJECT_END)
      // We're done
      break;
    // Read the first attribute name
    std::string attrName;
    if (is_copy_assignable<
            typename std::iterator_traits<decltype(status.p)>::reference()>())
      attrName = decodeStringInPlace(status);
    else
      attrName = decodeString(status);
    auto b4 = p;
    onAttribute(std::move(attrName));
    assert(b4 == p); // onAttribute must not consume anything
    // Read the ':' separator
    require(COLON, status);
    // Read the value
    token = require(valueTokens(), status);
#ifndef NDEBUG
    b4 = p;
    onVal(token);
    assert(b4 != p); // onVal must consume one value
#else
    onVal(token);
#endif
    // Read the comma or the end of the object
    token = require({COMMA, OBJECT_END}, status);
    // We're done
    if (token == OBJECT_END)
      break;
  }
}
}
