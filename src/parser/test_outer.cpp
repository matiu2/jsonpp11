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

    it("2: Can read COMMA", [&]() {
      std::string json{"   ,xxx   "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(COMMA));
    });

    it("3: Can read COLON", [&]() { assert(false); });
    it("4: Can read null", [&]() {
      assert(false);
    });
    it("5: Can read boolean", [&]() {
      assert(false);
    });
    it("6: Can read array", [&]() {
      assert(false);
    });
    it("7: Can read ARRAY_END", [&]() { assert(false); });
    it("8: Can read object", [&]() { assert(false); });

    it("9: Can read OBJECT_END", [&]() { assert(false); });
    it("10: Can read number", [&]() { assert(false); });
    it("11: Can read string", [&]() { assert(false); });
    it("12: Can read ERROR", [&]() { assert(false); });
  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
