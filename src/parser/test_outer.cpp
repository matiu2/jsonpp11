/// Tests the outer parser

#include <bandit/bandit.h>
#include <sstream>

#include "outer.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  describe("The number parser", [&]() {

    using Status = json::Status<std::string::const_iterator>;

    it("1: Can read HIT_END", [&]() {
      std::string json{""};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(HIT_END));
    });

    it("2: Can read COMMA", [&]() {});

    it("3: Can read COLON", [&]() {

    });

    it("4: Can read null", [&]() {

    });
    it("5: Can read boolean", [&]() {

    });
    it("6: Can read array", [&]() {

    });
    it("7: Can read ARRAY_END", [&]() {});
    it("8: Can read object", [&]() {});

    it("9: Can read OBJECT_END", [&]() {});
    it("10: Can read number", [&]() {});
    it("11: Can read string", [&]() {});
    it("12: Can read ERROR", [&]() {});
  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
