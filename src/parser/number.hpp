/// Parses JSON numbers
#pragma once

#include "error.hpp"
#include "../utils.hpp"
#include "status.hpp"

#include <type_traits>

namespace json {

/// Converts the components of a json number into a c++ number type
template <typename NumberType, typename UIntType>
inline NumberType json_num2cpp_num(bool isNeg, UIntType int_part, int expPart) {

  static_assert(std::is_arithmetic<NumberType>::value,
                "We can only generate number types");
  static_assert(std::is_signed<NumberType>::value,
                "As JSON can generate signed numbers, we should read them as "
                "signed numbers so as not to corrupt information silently");

  static_assert(
      std::is_integral<UIntType>::value,
      "Should be an unsigned int as big as is needed to not loose data");
  static_assert(
      std::is_arithmetic<UIntType>::value,
      "Should be an unsigned int as big as is needed to not loose data");
  static_assert(
      !std::is_signed<UIntType>::value,
      "Should be an unsigned int as big as is needed to not loose data");

  NumberType result = int_part;
  if (expPart < 0)
    while (expPart++ < 0)
      result *= 0.1;
  else
    while (expPart-- > 0)
      result *= 10;
  return isNeg ? -result : result;
}

/// Parses a JSON number
/// @tparam iterator an Input Iterator to chars
/// @tparam Output the output number type
/// @tparam iterator_traits iterator traits for 'iterator'
/// @param p Points to the first character of the number
/// @param pe Points to one past the end of the JSON input
/// @param output will be filled with the value of the number we parse
template <typename Output, typename Status>
inline Output readNumber(Status &status) {

  static_assert(is_valid_status<Status>(), "The status object must have "
                                           "iterators p, pe, and an error "
                                           "thrower function; onError");
  static_assert(std::is_arithmetic<Output>::value,
                "We can only generate number types");
  static_assert(std::is_signed<Output>::value,
                "As JSON can generate signed numbers, we should read them as "
                "signed numbers so as not to corrupt information silently");

  auto& p = status.p;
  const auto& pe = status.pe;
  const auto& onError = status.onError;

  // Check that we have some input
  if (p == pe)
    onError("No number found. At end of input", p);

  // Types ////////////////////

  /// These are the tokens that we expect when parsing a number
  enum Token {
    digit = 0, // Any digit 0..9
    // The next 4 tokens are literals
    positive = '+',
    negative = '-',
    dot = '.',
    exponent = 'e',
    END = ' ', // End of the number; either p == pe .. or we hit an unexpected
               // character (like a space for example)
  };

  // Varibales ////////////////////

  bool intIsNeg = false;          // true if the int part is negative
  bool expIsNeg = false;          // true if the exponent part is negative
  unsigned long long intPart = 0; // The integer part of the number
  int expPart1 = 0; // The inferred exponent part gotten from counting the
                    // decimal digits
  int expPart2 = 0; // The explicit exponent part from the number itself,
                    // added to the inferred exponent part
  bool gotAtLeastOneDigit = false;

  // Helper functions ////////////////////

  /// Gets the next token

  auto getToken = [&]() -> Token {
    switch (*p) {
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
      return digit;
    case '-':
      return negative;
    case '+':
      return positive;
    case '.':
      return dot;
    case 'e':
    case 'E':
      return exponent;
    default:
      return END;
    }
  };

  /// Records a single integer
  auto recordInt = [&]() {
    gotAtLeastOneDigit = true;
    intPart *= 10;
    intPart += *p - '0';
    ++p;
  };

  /// Records a single digit of the decimal part of the number
  auto recordDecimal = [&]() {
    intPart *= 10;
    intPart += *p - '0';
    --expPart1; // the 'actual' end exponent will be way at the end
    ++p;
  };

  /// Records a single digit of the exponent part of the number
  auto recordExponent = [&]() {
    expPart2 *= 10;
    expPart2 += *p - '0';
    ++p;
  };

  /// Reads the entire decimale part of the number
  auto readDecimalPart = [&]() {
    ++p; // Skip over the '.'
    while (p != pe) {
      switch (Token token = getToken()) {
      case digit:
        recordDecimal();
        break;
      case dot:
        onError("Second '.' found in a number", p);
      case exponent:
        return token;
      case negative:
        onError("Didn't expect a '-' in the middle of a number", p);
      case positive:
        onError("Didn't expect a '+' in the middle of a number", p);
      default:
        return END;
      };
    }
    return END;
  };

  /// Reads the entire exponent part of the JSON number
  auto readExponentPart = [&]() {
    // See if the first thing after the 'e' is a positive or minus sign
    ++p;
    switch (getToken()) {
    case negative:
      expIsNeg = true; // break; omitted here on purpose
    case positive:
      ++p; // break; omitted here on purpose
    case digit:
      break;
    default:
      onError("Expected a '+', '-', or a digit after the 'e' for exponent", p);
    }
    // Now read the rest of the exponent digits
    while (p != pe) {
      Token token;
      switch (token = getToken()) {
      case digit:
        recordExponent();
        break;
      case dot:
        onError("'.' found in a exponent", p);
      case exponent:
        onError("Second 'e' for exponent found in number", p);
      case negative:
        onError("Didn't expect a '-' in the middle of a number", p);
      case positive:
        onError("Didn't expect a '+' in the middle of a number", p);
      default:
        return END;
      }
    }
    return END;
  };

  /// Reads the entire integer part of the number
  auto readIntegerPart = [&]() -> Token {
    // Read the first digit
    Token token = getToken();
    switch (token) {
    case digit:
      recordInt();
      break;
    case negative:
      ++p;
      intIsNeg = true;
      break;
    default:
      onError("Expected a digit or a '-'", p);
    };
    // Read the rest of the integer part
    while (p != pe) {
      switch (token = getToken()) {
      case digit:
        recordInt();
        break;
      case dot:
        return readDecimalPart();
      case exponent:
        return readExponentPart();
      case negative:
        onError("Didn't expect a '-' in the middle of a number", p);
      case positive:
        onError("Didn't expect a '+' in the middle of a number", p);
      default:
        return END;
      };
    }
    return END;
  };
  /// Turns all our gathered data into useable result
  auto make_number = [&]() -> Output {
    // parse the string
    if (gotAtLeastOneDigit) {
      long expPart = expIsNeg ? expPart1 - expPart2 : expPart1 + expPart2;
      return json_num2cpp_num<Output>(intIsNeg, intPart, expPart);
    } else {
      // Might reach here if we find for example, a standalone + or - in the
      // json
      onError("Couldn't read a number", p);
    }
    assert("Code flow should never get here. onError should throw");
    return (Output)0;
  };

  // Actual Parsing Code ////////////////////

  Token token = readIntegerPart(); // First part of the number

  // Now we should see a decimal or an exponent part
  bool haveExponent = false;
  switch (token) {
  case dot:
    token = readDecimalPart(); // 2nd part of the number
    break;
  case exponent:
    token = readExponentPart();
    haveExponent = true;
    break;
  case END:
    return make_number();
  default:
    assert("Should never reach here");
    onError("Unexpected token in number", p);
  };

  switch (token) {
  case exponent:
    if (haveExponent)
      onError("found two exponents in a json number", p);
    token = readExponentPart();
    break;
  case END:
    return make_number();
  default:
    assert("Should never get here. All error conditions should have been "
           "handled above");
    onError("Unexpected token in number", p);
  }

  assert("Should never get here. All error conditions should have been "
         "handled above");
  return make_number();
}
}
