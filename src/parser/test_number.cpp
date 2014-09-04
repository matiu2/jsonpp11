/// Tests parsing of json numbers

#include <bandit/bandit.h>
#include <sstream>

#include "number.hpp"
#include "error.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  describe("The number parser", [&]() {

    it("1.0 Can read zero", [&]() {
      const std::string json = "0";
      int result = parseNumber<int>(json.cbegin(), json.cend());
      AssertThat(result, Equals(0));
    });

    it("1.1 Can read a number", [&]() {
      const std::string json = "123456789";
      int result = parseNumber<int>(json.cbegin(), json.cend());
      AssertThat(result, Equals(123456789));
    });

    it("1.2 Can read decimal", [&]() {
      const std::string json = "1.213";
      auto result = parseNumber<double>(json.cbegin(), json.cend());
      AssertThat(result, EqualsWithDelta(1.213, 0.0001));
    });

    it("1.3 Can read exponent", [&]() {
      const std::string json = "9999e02";
      auto result = parseNumber<int>(json.cbegin(), json.cend());
      AssertThat(result, Equals(999900));
    });

    it("1.4 Can read decimal and exponent", [&]() {
      const std::string json = "99.99e02";
      auto result = parseNumber<int>(json.cbegin(), json.cend());
      AssertThat(result, Equals(9999));
    });

    it("1.5 Can read decimal and exponent", [&]() {
      const std::string json = "0.9999e02";
      auto result = parseNumber<double>(json.cbegin(), json.cend());
      AssertThat(result, EqualsWithDelta(99.99, 0.001));
    });

    it("1.6 Can read a negative exponent", [&]() {
      const std::string json = "9999E-2";
      auto result = parseNumber<double>(json.cbegin(), json.cend());
      AssertThat(result, EqualsWithDelta(99.99, 0.001));
    });

    it("1.7 Can read a negative decimal exponent", [&]() {
      const std::string json = "999.9e-2";
      auto result = parseNumber<double>(json.cbegin(), json.cend());
      AssertThat(result, EqualsWithDelta(9.999, 0.001));
    });

  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
