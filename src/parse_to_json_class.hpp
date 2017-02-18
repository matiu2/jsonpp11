/// Parses incoming json into json_classes
#pragma once

#include "json_class.hpp"

#include "parser/array.hpp"
#include "parser/error.hpp"
#include "parser/number.hpp"
#include "parser/object.hpp"
#include "parser/outer.hpp"
#include "parser/status.hpp"
#include "parser/string.hpp"
#include "parser/utils.hpp"

#include <cassert>

namespace json {

template <typename Status>
inline JSON readValue(Status& status, Token token=ERROR) {
  static_assert(is_valid_status<Status>(), "You need to pass a status object to this function");
  static_assert(is_forward_iterator<typename Status::iterator>(),
                "string.hpp needs to copy the iterators and increment the copies "
                "without affecting the original");
  if (token == ERROR)
    token = require(valueTokens(), status);
  switch (token) {
  case null:
    readNull(status);
    return {};
  case boolean:
    return {readBoolean(status), 0};
  case array: {
    JList result;
    readArray(status, [&](Token t) {
      result.push_back(readValue(status, t));
    });
    return result;
  }
  case object: {
    JMap result;
    std::string lastAttr;
    readObject(status, [&](std::string attr) { lastAttr = attr; },
    [&](Token t) {
      result[lastAttr] = readValue(status, t);
    });
    return result;
  }
  case number:
    return readNumber<double>(status);
  case string:
    return decodeString(status);
  case HIT_END:
  case COMMA:
  case COLON:
  case ARRAY_END:
  case OBJECT_END:
  case ERROR:
    assert("Code shouldn't reach here because 'requrie' should throw on bad tokens");
    return ERROR;
  }
  assert("Code shouldn't reach here because 'requrie' should throw on bad tokens");
  return ERROR;
}

/**
* @brief Reads json using iterators. Outputs a json::JSON object
*
* @tparam Iterator The type of iterator that we'll be reading from
* @param jsonStart The start of the json stream
* @param jsonEnd The end of the json stream
*
* @return The read object
*/
template <typename Iterator>
JSON readValue(Iterator jsonStart, Iterator jsonEnd,
               ErrorThrower<Iterator> onError = throwError<Iterator>) {
  auto status = make_status(jsonStart, jsonEnd, onError);
  return readValue(status);
}

template <typename T>
JSON readValue(const T &source,
               ErrorThrower<Iterator> onError = throwError<Iterator>) {
  return readValue(source.begin(), source.end(), throwError);
}
}
