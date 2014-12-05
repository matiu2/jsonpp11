//// Tests that we can read an array
#include <bandit/bandit.h>

#include <sstream>
#include <string>
#include <iterator>

#include "outer.hpp"
#include "status.hpp"
#include "number.hpp"
#include "array.hpp"
#include "LocatingIterator.hpp"

using namespace bandit;
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
  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
