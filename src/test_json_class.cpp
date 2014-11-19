/// Tests all aspects of json parsing and generation

#include <bandit/bandit.h>
#include <sstream>

#include "json_class.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  std::stringstream output;

  before_each([&]() { output.str(""); });

  describe("The JSON model", [&]() {

    it("1.1. Can be null", [&]() {
      JSON j;
      AssertThat(j.isNull(), Equals(true));
      AssertThat(j.whatIs(), Equals(JSON::null));
      output << j;
      AssertThat(output.str(), Equals("null"));
    });

    it("1.2. Can be true", [&]() {
      JSON j = JBool(true);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat((bool)j, Equals(true));
      output << j;
      AssertThat(output.str(), Equals("true"));
    });

    it("1.3. Can be false", [&]() {
      JSON j = JBool(false);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::boolean));
      AssertThat((bool)j, Equals(false));
      output << j;
      AssertThat(output.str(), Equals("false"));
    });

    it("1.4. Can be an int", [&]() {
      JSON j(2);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((int)j, Equals(2));
      output << j;
      AssertThat(output.str(), Equals("2"));
    });

    it("1.5. Can be a double", [&]() {
      JSON j(3.1415927);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::number));
      AssertThat((long double)j, Equals(3.1415927));
      output << j;
      AssertThat(output.str(), Equals("3.14159"));
    });

    it("1.6. Can be a string", [&]() {
      JSON j("Hello there");
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::text));
      AssertThat((const std::string &)j, Equals("Hello there"));
      output << j;
      AssertThat(output.str(), Equals("\"Hello there\""));
    });

    it("1.6.5 Can be a const char *", [&]() {
      const char *input = "Hello there";
      JSON j(input);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::text));
      AssertThat((const std::string &)j, Equals("Hello there"));
      output << j;
      AssertThat(output.str(), Equals("\"Hello there\""));
    });

    it("1.7. Reads in UTF8", [&]() {
      constexpr const char *s = u8"ᚠᛇᚻ᛫ᛒᛦᚦ᛫ᚠᚱᚩᚠᚢᚱ᛫ᚠᛁᚱᚪ᛫ᚷᛖᚻᚹᛦᛚᚳᚢᛗ";
      JSON j(s);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::text));
      AssertThat((const std::string &)j, Equals(s));
      output << j;
      AssertThat(output.str(),
                 Equals("\"\u16A0\u16C7\u16BB\u16EB\u16D2\u16E6\u16A6\u16EB"
                        "\u16A0\u16B1\u16A9\u16A0\u16A2\u16B1\u16EB\u16A0"
                        "\u16C1\u16B1\u16AA\u16EB\u16B7\u16D6\u16BB\u16B9"
                        "\u16E6\u16DA\u16B3\u16A2\u16D7\""));
    });

    it("1.8. Reads in a list", [&]() {
      JSON j(JList{{}, {"String"}, {4.0}});
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::list));
      const JList &list = j;
      AssertThat(list, HasLength(3));
      output << j;
      AssertThat(output.str(), Equals("[null,\"String\",4]"));
    });

  });

  describe("The JSON model as a map", [&]() {

    JSON input(JMap{
        {"Lot 1",
         JMap{{"make", {"hillman"}}, {"model", {"Hunter"}}, {"year", {1974}}}},
        {"Lot 2", JMap{{"make", {"porsche"}},
                       {"model", {"Cayenne"}},
                       {"year", {1982}}}}});

    it("2.1. Reads in a map", [&]() {
      JSON j(input);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::map));
      const JMap &map = j;
      AssertThat(map.size(), Equals((size_t)2));
      output << j;
      AssertThat(
          output.str(),
          Equals(R"({"Lot )"
                 R"(1":{"make":"hillman","model":"Hunter","year":1974},"Lot )"
                 R"(2":{"make":"porsche","model":"Cayenne","year":1982}})"));
    });

    it("2.2. Has map access", [&]() {
      JSON j(input);
      AssertThat(j.isNull(), Equals(false));
      AssertThat(j.whatIs(), Equals(JSON::map));
      JSON &lot1 = j.at("Lot 1");
      AssertThat(lot1.whatIs(), Equals(JSON::map));
      JSON &jyear = lot1.at("year");
      // TODO: make a test that calls .at("Something that doesn't exist");
      AssertThat(jyear.whatIs(), Equals(JSON::number));
      int year(lot1["year"]);
      AssertThat(year, Equals(1974));
      AssertThat(static_cast<int>(j["Lot 1"]["year"]), Equals(1974));
    });

    it("2.3. Throws when it can't find a map element", [&]() {
      JSON j(input);
      AssertThrows(std::out_of_range, j.at("Something that doesn't exist"));
    });

    it("2.3. Allows adding map entries using [] operators", [&]() {
      JSON j(input);
      j["Lot 3"] = {"Not here"};
      std::string lot3 = j.at("Lot 3");
      AssertThat(lot3, Equals("Not here"));
    });

    it("2.4. Allows altering map entries using [] operators", [&]() {
      JSON j(input);
      j["Lot 1"]["year"] = {1960};
      int year{j["Lot 1"]["year"]};
      AssertThat(year, Equals(1960));
    });

    it("2.5. can export with jsonToHomogenousMap", [&]() {
      JSON j{JMap{{"Energy (Kj)", 11}, {"Sugar", 1.1}, {"Fat", 20}, {"Potasium", 0.2}}};
      std::map<std::string, double> expected = {{"Energy (Kj)", 11}, {"Sugar", 1.1}, {"Fat", 20}, {"Potasium", 0.2}};
      std::map<std::string, double> got = jsonToHomogenousMap<double>(j);
      AssertThat(got, Equals(expected));
    });

  });

  describe("The model as a list", [&]() {

    JSON input{JList{1, 2, 3, 4, 5}};

    it("3.1. Reads in a list", [&]() {
      std::vector<int> value;
      JList& l(input);
      value.reserve(l.size());
      for (JSON& j : l)
        value.push_back((int)j);
      AssertThat(value, Equals(std::vector<int>{1, 2, 3, 4, 5}));
    });

    it("3.2. Allows at() access to a list", [&]() {
      int x{input.at(2)};
      AssertThat(x, Equals(3));
    });

    it("3.3. Allows [] assignment to a list", [&]() {
      input[2] = 999;
      AssertThat((int)input.at(2), Equals(999));
    });

    it("3.4. On a list, at(999) throws out_of_range", [&]() {
      AssertThrows(std::out_of_range, input.at(999));
    });

    it("3.5 can export via jsonToHomogenousList", [&]() {
      JSON j{JList{900.99, 989.99, 2000}};
      std::vector<double> exported = jsonToHomogenousList<double>(j);
      AssertThat(exported, Equals(std::vector<double>{900.99, 989.99, 2000}));
    });

  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
