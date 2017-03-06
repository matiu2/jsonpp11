#include <bandit/bandit.h>
#include <sstream>
#include <string>
#include <iterator>

#include "status.hpp"
#include "string.hpp"

using namespace bandit;
using namespace snowhouse;
using namespace json;

// Just for snowhouse 'Equals' to work
namespace std {

template <typename T, typename Traits=std::iterator_traits<T>>
bool operator ==(const std::basic_string<typename T::value_type>& a, const json::string_reference<T>& b) {
  return b == a;
}

/**
* @brief Allows us to print string_reference instances
*
* @tparam T The iterator type for the string reference
* @param o The ostream that we are printing to
* @param s The string_reference that we are prnting
*
* @return A reference to the ostream that we were passed in
*/
template <typename T>
ostream& operator <<(ostream& o, json::string_reference<T> s) {
  auto out = ostream_iterator<char>(o);
  std::copy(s.cbegin(), s.cend(), out);
  return o;
}

}


go_bandit([]() {

  std::stringstream output;

  describe("decodeString with random access iterators", [&]() {

    it("1.1. Can be empty", [&]() {
      std::string input = R"(")";
      auto status = make_status(input.begin(), input.end());
      auto output = json::decodeString(status);
      AssertThat(output.begin(), Equals(output.end()));
    });

    it("1.2. Can be a single char", [&]() {
      std::string input = R"(x")";
      std::string expected = "x";
      auto status = make_status(input.begin(), input.end());
      auto output = json::decodeString(status);
      AssertThat(expected, Is().EqualTo(output)); // Needs to be this way around to use the correct snowhouse template
    });

    it("1.3. Can be several chars", [&]() {
      std::string input = R"(abcdefg")";
      std::string expected = "abcdefg";
      std::string expected2 = "(1241142";
      auto status = make_status(input.begin(), input.end());
      auto output = json::decodeString(status);
      AssertThat(output, Equals(expected));
      AssertThat(output, Is().Not().EqualTo(expected2));
    });

    it("1.4. Can parse all escape characters", [&]() {
      std::string input = R"(\b\f\n\r\t")";
      std::string expected = "\b\f\n\r\t";
      auto status = make_status(input.begin(), input.end());
      auto output = json::decodeString(status);
      AssertThat(output, Equals(expected));
    });

    it("1.5. Can parse unicode char", [&]() {
      std::string input = R"(\u03E0")";
      std::string expected = u8"\u03E0";
      auto status = make_status(input.begin(), input.end());
      auto output = json::decodeString(status);
      AssertThat(output, Equals(expected));
    });

    it("1.6. Can parse the largest 16 bit unicode char", [&]() {
      std::string input = R"(\uFFFF")";
      std::string expected = u8"\uffff";
      auto status = make_status(input.begin(), input.end());
      auto output = json::decodeString(status);
      AssertThat(output, Equals(expected));
    });

    it("1.7. Can detect a bad unicode char", [&]() {
      std::string input = R"(\u03E01111111111111")";
      auto status = make_status(input.begin(), input.end());
      AssertThrows(ParserError, json::decodeString(status));
    });

    it("1.8. Can parse a 32 bit unicode char", [&]() {
      std::string input = R"(\uD834\uDD1E")";
      std::string expected = u8"\U0001D11E";
      auto status = make_status(input.begin(), input.end());
      auto output = json::decodeString(status);
      AssertThat(output, Equals(expected));
    });

    it("1.9. Can parse a char* input type", [&]() {
      std::string data = R"(\uD834\uDD1E")";
      char* input = new char[data.size()];
      std::copy(data.begin(), data.end(), input);
      std::string expected = u8"\U0001D11E";
      auto status = make_status(input, &input[data.size()]);
      json::string_reference<char*> output = json::decodeStringLight(status);
      AssertThat(std::string(output), Equals(expected));
      delete[] input;
    });

  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
