//// Tests that we can read an object
#include <bandit/bandit.h>

#include "outer.hpp"
#include "status.hpp"
#include "number.hpp"
#include "object.hpp"
#include "LocatingIterator.hpp"

#include <sstream>
#include <string>
#include <iterator>
#include <cassert>
#include <iostream>

using namespace bandit;
using namespace snowhouse;
using namespace json;

go_bandit([]() {

  std::stringstream output;

  describe("readObject", [&]() {

    it("1.0 Can read an empty object", []() {
      std::string json("{}");

      auto s = json::make_status(makeLocating(json.begin()), makeLocating(json.end()));
  
      // Line it up for reading the object
      Token token = getNextOuterToken(s);
      AssertThat(token, Equals(object));

      bool gotAttr = false;
      bool gotVal = false;

      auto onAttribute = [&](std::string &&) { gotAttr = true; };
      auto onVal = [&](json::Token) { gotVal = true; };

      try {
        readObject(s, onAttribute, onVal);
      } catch (std::exception& e) {
        std::cerr << "JSON: " <<  json;
        throw;
      }
  
      AssertThat(gotAttr, Equals(false));
      AssertThat(gotVal, Equals(false));
    }); 

    it("2.0 Can read a simple object", []() {
      std::string json(R"(  { "number": 12 } )");

      auto s = json::make_status(makeLocating(json.begin()), makeLocating(json.end()));
  
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

      try {
        readObject(s, onAttribute, onVal);
      } catch (std::exception& e) {
        std::cerr << "JSON: " <<  json;
        throw;
      }
  
      AssertThat(attrName, Equals("number"));
      AssertThat(token, Equals(number));
      AssertThat(value, Equals(12));

      AssertThat(s.p, Equals(json.begin() + 18)); // One past the '}'
    }); 

    it("3.0 Can read an object", []() {
      std::string json(R"(  { "name": "smith", "age": 12, "rating": 2.4, "active": true } )");

      auto s = json::make_status(makeLocating(json.begin()), makeLocating(json.end()));
  
      // Line it up for reading the object
      Token token = getNextOuterToken(s);
      AssertThat(token, Equals(object));

      std::vector<std::string> attributeNames;
      struct {
        std::string name;
        int age;
        double rating;
        bool active;
      } data;

      auto onAttribute =
          [&](std::string &&attr) { attributeNames.emplace_back(attr); };
      auto onVal = [&](json::Token t) {
        assert(attributeNames.size() > 0);
        if (attributeNames.back() == "name") {
          assert (t == json::string);
          data.name = decodeString(s);
        } else if (attributeNames.back() == "age") {
          assert (t == number);
          data.age = readNumber<int>(s);
        } else if (attributeNames.back() == "rating") {
          assert (t == number);
          data.rating = readNumber<double>(s);
        } else if (attributeNames.back() == "active") {
          assert (t == boolean);
          data.active = readBoolean(s);
        } else {
          assert(false); // Code should never get here
        }
      };

      try {
        readObject(s, onAttribute, onVal);
      } catch (std::exception& e) {
        std::cerr << "JSON: " <<  json;
        throw;
      }
  


      AssertThat(attributeNames, EqualsContainer(decltype(attributeNames)({"name", "age", "rating", "active"})));
  
      AssertThat(data.name, Equals("smith"));
      AssertThat(data.age, Equals(12));
      AssertThat(data.rating, EqualsWithDelta(2.4, 0.01));
      AssertThat(data.active, Equals(true));

    });

  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
