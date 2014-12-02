//// Tests that we can read an object
#include <bandit/bandit.h>

#include <sstream>
#include <string>
#include <iterator>

#include "outer.hpp"
#include "status.hpp"
#include "number.hpp"
#include "object.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  std::stringstream output;

  describe("readObject", [&]() {

    it("1.0 Can read a simple object", []() {
      std::string json(R"(  { "number": 12 } )");

      auto s = json::make_status(json.begin(), json.end());
  
      // Line it up for reading the object
      Token token = getNextOuterToken(s);
      AssertThat(token, Equals(object));

      std::string attrName;
      int value;

      auto onAttribute = [&](std::string&& attr) { attrName = attr; };
      auto onVal = [&](json::Token t) {
        token = t;
        if (token == number)
          value = readNumber<int>(s);
      };

      readObject(s, onAttribute, onVal);
  
      AssertThat(attrName, Equals("number"));
      AssertThat(token, Equals(number));
      AssertThat(value, Equals(12));

      AssertThat(s.p, Equals(json.begin() + 18)); // One past the '}'
    }); 

  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
