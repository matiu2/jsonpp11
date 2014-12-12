/// General utilities for parsing code
#pragma once

#include <set>

#include "status.hpp"
#include "outer.hpp"

#include <cassert>

namespace json {

/// Throws an error if the next token is not acceptable
/// @param acceptable Tokens that don't cause an error
/// @param parser status
/// return 
template <typename Status>
Token require(std::set<Token> expected, Status& status) {
  Token got = getNextOuterToken(status);
  if (expected.find(got) != expected.end())
    return got;
  // Make up the error message
  std::stringstream msg;
  msg << "Expected ";
  // List of expected tokens
  for (auto token : expected)
    msg << "'" << (char)token << "', ";
  msg.seekp(-2, std::ios_base::cur); // Remove the last ', '
  if (got == HIT_END)
    msg << "' but hit the end of input";
  else
    msg << "' but got '" << (char)got << "' instead";
  status.onError(msg.str());
  assert(true); // Code should never reach here. onError should throw an
                // expection
  return ERROR;
}

/**
* @brief Require a token with one single good possible value
*
* @param required The token that we need
* @param status The parser status
*
* @return the token that we found
* @throws calls onError for any other token
*/
template <typename Status>
Token require(Token required, Status& status) {
  return require(std::set<Token>{required}, status);
}

const std::set<Token> valueTokens() {
  return {null, boolean, array, object, number, string};
}

}
