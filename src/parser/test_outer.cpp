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
