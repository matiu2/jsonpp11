//// Tests that we can read an array
#include <bandit/bandit.h>

#include <sstream>
#include <string>
#include <iterator>

#include "outer.hpp"
#include "status.hpp"
#include "number.hpp"
#include "string.hpp"
#include "array.hpp"
#include "LocatingIterator.hpp"

using namespace bandit;
using namespace snowhouse;
using namespace json;

go_bandit([]() {

  std::stringstream output;

  describe("readObject", [&]() {

    it("1.0 Can read an empty array", []() {
      std::string json = "[]";

      auto s = json::make_status(makeLocating(json.begin()), makeLocating(json.end()));

      // Line it up for reading the object
      Token token = getNextOuterToken(s);
      AssertThat(token, Equals(array));

      bool called = false;
      auto onVal = [&](Token) { called = true; };
      readArray(s, onVal);

      AssertThat(s.p, Equals(s.pe));
    }); 

    it("2.0 Can read a single int array", []() {
      std::string json = "[1]";

      auto s = json::make_status(makeLocating(json.begin()), makeLocating(json.end()));

      // Line it up for reading the object
      Token token = getNextOuterToken(s);
      AssertThat(token, Equals(array));

      int val = 0;

      auto onVal = [&](Token t) {
        token = t;
        val = readNumber<int>(s);
      };

      readArray(s, onVal);

      AssertThat(token, Equals(number));
      AssertThat(val, Equals(1));
      AssertThat(s.p, Equals(s.pe));
    }); 

    it("3.0 Can read a mixed type array", []() {
      std::string json = R"([1, "Jasper", 3.4, false])";

      auto s = json::make_status(makeLocating(json.begin()), makeLocating(json.end()));

      // Line it up for reading the object
      Token token = getNextOuterToken(s);
      AssertThat(token, Equals(array));

      std::tuple<int, std::string, double, bool> data;
      int index=0;
      using Status = decltype(s);
      auto readers = std::make_tuple(readNumber<int, Status>, decodeString<Status>, readNumber<double, Status>, readBoolean<Status>);
      std::vector<Token> tokens;

      auto onVal = [&](Token t) {
        token = t;
        tokens.push_back(t);
        switch (index) {
        case 0:
          std::get<0>(data) = std::get<0>(readers)(s);
          break;
        case 1:
          std::get<1>(data) = std::get<1>(readers)(s);
          break;
        case 2:
          std::get<2>(data) = std::get<2>(readers)(s);
          break;
        case 3:
          std::get<3>(data) = std::get<3>(readers)(s);
          break;
        }
        ++index;
      };

      readArray(s, onVal);

      AssertThat(std::get<0>(data), Equals(1));
      AssertThat(std::get<1>(data), Equals("Jasper"));
      AssertThat(std::get<2>(data), EqualsWithDelta(3.4, 0.01));
      AssertThat(std::get<3>(data), Equals(false));
      AssertThat(tokens, EqualsContainer(decltype(tokens){number, string, number, boolean}));
      AssertThat(s.p, Equals(s.pe));
    }); 
  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
