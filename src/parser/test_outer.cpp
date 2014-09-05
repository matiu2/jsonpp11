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

    it("3: Can read COLON", [&]() { 
      std::string json{"   :   "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(COLON));
    });

    it("4: Can read null", [&]() {
      AssertThat(false, Equals(true));
    });
    it("5: Can read boolean", [&]() {
      AssertThat(false, Equals(true));
    });
    it("6: Can read array", [&]() {
      AssertThat(false, Equals(true));
    });
    it("7: Can read ARRAY_END", [&]() { 
      AssertThat(false, Equals(true));
    });
    it("8: Can read object", [&]() { 
      AssertThat(false, Equals(true));
    });

    it("9: Can read OBJECT_END", [&]() { 
      AssertThat(false, Equals(true));
    });
    it("10: Can read number", [&]() { 
      AssertThat(false, Equals(true));
    });
    it("11: Can read string", [&]() { 
      AssertThat(false, Equals(true));
    });
    it("12: Can read ERROR", [&]() { 
      AssertThat(false, Equals(true));
    });
  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
