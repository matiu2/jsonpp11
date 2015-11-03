/// Parser exception
#pragma once

#include <type_traits>
#include <cstring>
#include <functional>
#include <sstream>

#include "../utils.hpp"

namespace json {

// An iterator may provide an 'int row; int col;' that specify the row and
// column of the input text that it's currently reading. If it does, we'll
// report that info when we throw the exception.

/// The base parser Error, doesn't return any location information
struct ParserError : std::runtime_error {
public:
  ParserError(std::string msg) : std::runtime_error(msg) {}
};

/// tparam P the Parser specialization that we refer to
class LocatingParserError : public ParserError {
private:
  static std::string make_msg(std::string msg, int row, int col) {
    std::stringstream result;
    result << msg << " at row " << row << " col " << col;
    // return msg + " at " + location;
    return result.str();
  }

public:
  LocatingParserError(std::string msg, int row, int col)
      : ParserError(make_msg(msg, row, col)), row(row), col(col) {}
  const int row;
  const int col;
};

// A couple of throwError template functions, that will help choose the correct
// ParserError class to throw

/// Throws the LocatingParserError
template <typename Iterator>
enable_if<has_location<Iterator>(), void> throwError(std::string msg, Iterator iter) {
  throw LocatingParserError(msg, iter.row, iter.col);
}

/// Throws the ParserError
template <typename Iterator>
enable_if<!has_location<Iterator>(), void> throwError(std::string msg, Iterator) {
  throw ParserError(msg);
}

/// A callable type that can throw errors for us
template <typename Iterator>
using ErrorThrower = std::function<void(std::string msg, Iterator)>;
}
