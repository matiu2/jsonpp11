/// Tests parsing of json numbers

#include <bandit/bandit.h>
#include <sstream>

#include "number.hpp"
#include "error.hpp"

using namespace bandit;
using namespace json;

struct LocatingIterator {
  int row;
  int col;
};

go_bandit([]() {

  describe("The number parser", [&]() {

    it("1.0 Can read 0", [&]() {
      static_assert(has_location<LocatingIterator>(), "LocatingIterator should have .row and .col");
      static_assert(!has_location<char*>(), "char* should not have .row and .col attributes");
      static_assert(!has_location<typename std::string::const_iterator>(), "String should not have .row and .col attributes");
      const std::string json = "0";
      int result = parseNumber<int>(json.cbegin(), json.cend());
      AssertThat(result, Equals(0));
    });

  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
