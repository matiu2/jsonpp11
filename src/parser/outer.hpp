/// Parses the outter part of the json, and the basic types:
/// null, true, false
#pragma once

#include "status.hpp"

namespace json {

/// The tokens that we look for when trying to determine the JSON type
enum Token {
  HIT_END = 0,
  COMMA = ',',
  COLON = ':',
  null = 'n',
  boolean = 't',
  array = '[',
  ARRAY_END = ']',
  object = '{',
  OBJECT_END = '}',
  number = '0',
  string = '"',
  ERROR = 'x'
};

/**
 * Finds the type of the next JSON in the stream
 *
 * @tparam Status Some kind of parser.hpp Status template instantiation
 * @param Status A reference to a valid parser status instance
 * @returns the token for the character in the JSON, ready to read the value of
 * the type
 */
template <typename Status>
inline Token
getNextOuterToken(Status& status) {

  static_assert(is_valid_status<Status>(), "The status object must have "
                                           "iterators p, pe, and an error "
                                           "thrower function; onError");

  auto& p = status.p;
  const auto& pe = status.pe;

  /// Skips over white space
  auto skipWS = [&]() {
    while ((p != pe)) {
      switch ((*p)) {
      case 9:
      case 10:
      case 13:
      case ' ':
        ++p;
        continue;
      default:
        return;
      }
    }
  };

  skipWS();
  while ((p != pe)) {
    switch ((*p)) {
    case '"':
      ++p;
      return string;
    case ',':
      ++p;
      return COMMA;
    case ':':
      ++p;
      return COLON;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return number;
    case '[':
      ++p;
      return array;
    case ']':
      ++p;
      return ARRAY_END;
    case 'f':
      return boolean;
    case 'n':
      ++p;
      return null;
    case 't':
      return boolean;
    case '{':
      ++p;
      return object;
    case '}':
      ++p;
      return OBJECT_END;
    default:
      // If we got here, it's because we hit the end of the stream, or found
      // an unexpected character in the json stream;
      return ERROR;
    }
  }
  return HIT_END;
}

// Reader functions for basic types ////////////////////

/**
 * Reads a 'null' value
 *
 * @tparam Status Some kind of parser.hpp Status template instantiation
 * @param Status A reference to a valid parser status instance
 * @param returns the new iterator position
 */
template <typename Status> inline void readNull(Status &status) {
  static_assert(is_valid_status<Status>(), "The status object must have "
                                           "iterators p, pe, and an error "
                                           "thrower function; onError");
  checkStaticString(status, "ull"); // The 'n' was already confirmed by getNextOuterToken
}

/**
 * Reads true or false
 *
 * @returns a boolean, true or false
 */
template <typename Status> inline bool readBoolean(Status &status) {
  static_assert(is_valid_status<Status>(), "The status object must have "
                                           "iterators p, pe, and an error "
                                           "thrower function; onError");
  switch (*status.p++) {
  case 't':
    checkStaticString(status, "rue");
    return true;
  case 'f':
    checkStaticString(status, "alse");
    return false;
  default:
    status.onError("Expected 'true' or 'false'", status.p);
  }
  assert("Code flow should never reach here, as onError should throw");
  return false;
}
}
