/// Tests parsing of json numbers

#include <bandit/bandit.h>
#include <sstream>

#include "number.hpp"
#include "error.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  describe("The number parser", [&]() {

    it("1.0 Can read 0", [&]() {
      const std::string json = "0";
      int result = parseNumber<int>(json.cbegin(), json.cend());
      AssertThat(result, Equals(0));
    });

    it("1.1 Can read 1.213", [&]() {
      const std::string json = "1.213";
      auto result = parseNumber<double>(json.cbegin(), json.cend());
      AssertThat(result, EqualsWithDelta(1.213, 0.0001));
    });
  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
