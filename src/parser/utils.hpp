/// General utilities for parsing code
#pragma once

#include "status.hpp"

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

/// @param status the parser status
/// @param alternative An acceptable alternative to a real token
/// @returns a real token and returns it, or errors if it can't
template <typename Status>
Token expectAnyRealType(Status& status, Token alternative=ERROR) {
  Token got = getNextOuterToken(status);
  if ((alternative != ERROR) && (got == alternative))
    return got;
  switch (got) {
  case null:
  case boolean:
  case array:
  case object:
  case number:
  case string:
    return got;
  default: {
    std::stringstream msg;
    if (got == HIT_END)
      msg << "Expected any real json type but hit the end of input";
    else
      msg << "Expected any real json type but got '" << (char)got
          << "' instead";
    status.onError(msg.str());
    assert(true); // Should never reach here as 'onError' should throw
    return ERROR;
  }
  };
}

}
