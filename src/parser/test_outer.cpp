/// Tests the outer parser

#include <bandit/bandit.h>
#include <iostream>

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
      AssertThat(status.p, Equals(status.pe));
    });

    it("2: Can read COMMA", [&]() {
      std::string json{"   ,xxx   "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(COMMA));
      AssertThat(status.p, Equals(json.cbegin() + 4));
      AssertThat(*status.p, Equals('x'));
    });

    it("3: Can read COLON", [&]() { 
      std::string json{"   :   "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(COLON));
      AssertThat(status.p, Equals(json.cbegin() + 4));
      AssertThat(*status.p, Equals(' '));
    });

    it("4: Can read null", [&]() {
      std::string json{"   null   "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(null));
      AssertThat(status.p,
                 Equals(json.cbegin() + 4)); // It should skip over the 'n'
      // Now read it
      readNull(status);
      AssertThat(status.p,
                 Equals(json.cbegin() +
                        7)); // It should skip over the whole word 'null'
      AssertThat(*status.p, Equals(' '));
    });

    it("5: Can read boolean", [&]() {
      std::string json{"   true   "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(boolean));
      AssertThat(status.p,
                 Equals(json.cbegin() + 3)); // It should not skip over the 't'
    });

    it("5.1: Can read true", [&]() {
      std::string json{"   true   "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(boolean));
      AssertThat(status.p,
                 Equals(json.cbegin() + 3)); // It should not skip over the 't'
      // Now read it
      bool value = readBoolean(status);
      AssertThat(value, Equals(true));
      AssertThat(status.p,
                 Equals(json.cbegin() +
                        7)); // It should skip over the whole word 'true'
      AssertThat(*status.p, Equals(' '));
    });

    it("5.2: Can read false", [&]() {
      std::string json{"   false   "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(boolean));
      AssertThat(status.p,
                 Equals(json.cbegin() + 3)); // It should not skip over the 'f'
      // Now read it
      bool value = readBoolean(status);
      AssertThat(value, Equals(false));
      AssertThat(status.p,
                 Equals(json.cbegin() +
                        8)); // It should skip over the whole word 'false'
      AssertThat(*status.p, Equals(' '));
    });

    it("6: Can read array start", [&]() {
      std::string json{" [blah, blah ]"};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(array));
      // It should put us one past the '['
      AssertThat(status.p, Equals(json.cbegin() + 2));
      AssertThat(*status.p, Equals('b'));
    });

    it("7: Can read ARRAY_END", [&]() { 
      std::string json{" ]x "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(ARRAY_END));
      // It should put us one past the ']'
      AssertThat(status.p, Equals(json.cbegin() + 2));
      AssertThat(*status.p, Equals('x'));
    });

    it("8: Can read object start", [&]() { 
      std::string json{R"( {"blah": blah })"};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(object));
      // It should put us one past the '{'
      AssertThat(status.p, Equals(json.cbegin() + 2));
      AssertThat(*status.p, Equals('"'));
    });

    it("9: Can read OBJECT_END", [&]() { 
      std::string json{" }x "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(OBJECT_END));
      // It should put us one past the ']'
      AssertThat(status.p, Equals(json.cbegin() + 2));
      AssertThat(*status.p, Equals('x'));
    });

    it("10: Can find a number", [&]() { 
      std::string json{" 123 "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(number));
      // It should put us at the '1'
      AssertThat(status.p, Equals(json.cbegin() + 1));
      AssertThat(*status.p, Equals('1'));
    });

    it("10.1: Can find a negative number", [&]() { 
      std::string json{" -9 "};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(number));
      // It should put us at the '-'
      AssertThat(status.p, Equals(json.cbegin() + 1));
      AssertThat(*status.p, Equals('-'));
    });

    it("11: Can find a string", [&]() { 
      std::string json{R"( "hello" )"};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(string));
      // It should put us one past the '"'
      AssertThat(status.p, Equals(json.cbegin() + 2));
      AssertThat(*status.p, Equals('h'));
    });

    it("12: Can find an ERROR", [&]() { 
      std::string json{R"( hello )"};
      Status status(json.cbegin(), json.cend());
      Token result = getNextOuterToken(status);
      AssertThat(result, Equals(string));
      // It should put us at the 'h'
      AssertThat(status.p, Equals(json.cbegin() + 1));
      AssertThat(*status.p, Equals('h'));
    });
  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
