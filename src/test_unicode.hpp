/// Tests the conversion of unicode

#include <bandit/bandit.h>
#include <string>

#include "unicode.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  describe("utf-16 Reader", [&]() {

    using Status = json::Status<std::string::const_iterator>;

    it("1.0 Can read an empty string", [&]() {
      std::u16string input = "";
      std::u32string expected = "";
      from16(input.cbegin(), std::back_inserter(expected));
      AssertThat(input, Equals(expected));
    });

  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
