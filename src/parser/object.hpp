#pragma once

#include "outer.hpp"
#include "number.hpp"
#include "string.hpp"
#include "../utils.hpp"
#include "utils.hpp"
#include "error.hpp"

#include <stdexcept>
#include <cassert>

namespace json {

template <typename Status> std::string best_string_decoder(Status &status) {
  BOOST_HANA_CONSTANT_ASSERT(is_valid_status(status));
  return hana::if_(is_forward_iterator(status.p) &&
                       is_output_iterator(status.p),
                   [&]() { return decodeStringInPlace(status); },
                   [&]() { return decodeString(status); })();
}

/// Convenience function to read an object.
/// @onAttribute will be called when an attribute name is read
/// @onVal will be read when the stream is ready to read a value. The function
///        needs to consume that value from the stream.
template <typename Status>
void readObject(Status &status, std::function<void(std::string &&)> onAttribute,
                std::function<void(Token)> onVal) {

  BOOST_HANA_CONSTANT_ASSERT(is_forward_iterator(status.p));

  auto& p = status.p;
  auto& pe = status.pe;
  while (p != pe) {
    // We need an attribute, or the end of the object
    Token token = require({OBJECT_END, string}, status);
    if (token == OBJECT_END)
      // We're done
      break;
    // Read the first attribute name
    std::string attrName = decodeString(status);
#ifndef NDEBUG
    // Forward iterators can be copied then compared
    auto b4 = p;
    onAttribute(std::move(attrName));
    assert(b4 == p); // onAttribute must not consume anything
#else
    onAttribute(std::move(attrName));
#endif
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
