#pragma once

#include "outer.hpp"
#include "number.hpp"
#include "string.hpp"
//#include "array.hpp"
#include "../utils.hpp"
#include "error.hpp"

#include <stdexcept>

namespace json {

/// @returns true if you got the token you expected
template <typename Status>
bool expect(Token expected, Token got, Status& status) {
  if (expected != got) {
    std::stringstream msg;
    if (got == HIT_END)
      msg << "Expected '" << (char)expected << "' but hit the end of input";
    else
      msg << "Expected '" << (char)expected << "' but got '" << (char)got
          << "' instead";
    status.onError(msg.str());
    return false;
  }
  return true;
}

/// @returns true if you got the type you wanted
template <typename Status>
bool expectAnyRealType(Status status) {
  auto got = getNextOuterToken(status);
  switch (got) {
  case null:
  case boolean:
  case array:
  case object:
  case number:
  case string:
    return true;
  default: {
    std::stringstream msg;
    if (got == HIT_END)
      msg << "Expected any real json type but hit the end of input";
    else
      msg << "Expected any real json type but got '" << (char)got
          << "' instead";
    status.onError(msg.str());
  }
  };
  return false;
}

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
    Token next = getNextOuterToken(status);
    // Read the attribute name or the end of the object
    if (next == OBJECT_END)
      // We're done
      break;
    if (!expect(string, next, status))
      break;
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
    if (!expect(COLON, getNextOuterToken(status), status))
      break;
    // Read the value
    if (!expectAnyRealType(status))
      break;
#ifndef NDEBUG
    b4 = p;
    onVal(next);
    assert(b4 != p); // onVal must consume one value
#else
    onVal(next);
#endif
    // Read the comma or the end of the object
    next = getNextOuterToken(status);
    if (next == OBJECT_END)
      // We're done
      break;
    if (!expect(COMMA, next, status))
      // We needed a comma to continue without error
      break;
  }
}
}
