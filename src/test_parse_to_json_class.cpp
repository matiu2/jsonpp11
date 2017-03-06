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
    it("1.0 - Can read a complex object", [&]() {
      std::ifstream file("sample.json");
      std::string json(std::istreambuf_iterator<char>(file.rdbuf()),
                       std::istreambuf_iterator<char>());
      JSON result = readValue(json.begin(), json.end());
      auto &access = result.at("access");
      auto &token = access["token"];
      std::string id = token.at("id");
      AssertThat(id, snowhouse::Equals("930fa23xxxxxxxxxxd711582ac0df492"));
    });
    it("1.1 - Can read a complex object v2", [&]() {
      std::ifstream f("sample2.json");
      std::string json(makeLocating(std::istreambuf_iterator<char>(f)),
                       makeLocating(std::istreambuf_iterator<char>{}));
      // noskipws(json);
      // auto start = makeLocating(std::istream_iterator<char>(json));
      // auto end = makeLocating(typename decltype(start)::original_type{});
      JSON result = readValue(json.begin(), json.end());
      std::string content_type = result.at("headers").at("Content-Type");
      AssertThat(content_type,
                 snowhouse::Equals("application/x-www-form-urlencoded"));
    });
  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
