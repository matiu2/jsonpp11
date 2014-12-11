//// Tests that we can read JSON into json::JSON objects
#include <bandit/bandit.h>

#include <sstream>
#include <string>
#include <iterator>

#include "parse_to_json_class.hpp"
#include "parser/outer.hpp"
#include "parser/status.hpp"
#include "parser/number.hpp"
#include "parser/object.hpp"
#include "parser/LocatingIterator.hpp"
#include "json_class.hpp"

#include <fstream>
#include <iterator>

using namespace bandit;
using namespace json;

go_bandit([]() {

  std::stringstream output;

  describe("parse to json", [&]() {
    it("Can write and read a complex object", [&]() {
      std::ifstream f("sample.json");
      std::string json(std::istreambuf_iterator<char>(f),
                       std::istreambuf_iterator<char>{});
      //noskipws(json);
      //auto start = makeLocating(std::istream_iterator<char>(json));
      //auto end = makeLocating(typename decltype(start)::original_type{});
      JSON result = readValue(json.begin(), json.end());
      auto& access = result.at("access");
      auto& token = access["token"];
      std::string id = token.at("id");
      AssertThat(id, Equals("930fa23xxxxxxxxxxd711582ac0df492"));
    });
  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
