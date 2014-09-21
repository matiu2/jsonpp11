/// Tests the conversion of unicode

#include <bandit/bandit.h>
#include <string>

#include "unicode.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  describe("utf-16 Reader", [&]() {

    it("1.0 Can read a simple unicode char", [&]() {
      std::u16string input = u"\u0466";
      std::u32string output = U"";
      std::u32string expected = U"\u0466";
      from16(input.cbegin(), std::back_inserter(output));
      AssertThat(output, Equals(expected));
    });

  });

  describe("utf-8 Reader", [&]() {

    it("2.0 Can read a simple unicode char", [&]() {
      std::string input = u8"\u0066";
      std::u32string output = U"";
      std::u32string expected = U"\u0066";
      from8(input.cbegin(), std::back_inserter(output));
      AssertThat(output, Equals(expected));
    });

    it("2.1 Can read a 2 byte encoded unicode char", [&]() {
      std::string input = u8"\u0466";
      std::u32string output = U"";
      std::u32string expected = U"\u0466";
      from8(input.cbegin(), std::back_inserter(output));
      AssertThat(output, Equals(expected));
    });

    it("2.2 Can read a 3 byte encoded unicode char", [&]() {
      std::string input = u8"\u0966";
      std::u32string output = U"";
      std::u32string expected = U"\u0966";
      from8(input.cbegin(), std::back_inserter(output));
      AssertThat(output, Equals(expected));
    });

    it("2.3 Can read the highest value char", [&]() {
      std::string input = u8"\U0001F8FF";
      std::u32string output = U"";
      std::u32string expected = U"\U0001F8FF";
      from8(input.cbegin(), std::back_inserter(output));
      AssertThat(output, Equals(expected));
    });

    it("2.4 Can read a 4 byte encoded char", [&]() {
      std::string input = u8"𰵀";
      std::u32string output = U"";
      std::u32string expected = U"𰵀";
      from8(input.cbegin(), std::back_inserter(output));
      AssertThat(output, Equals(expected));
    });

    it("2.5 Can read a 5 byte encoded char", [&]() {
      std::string input = u8"�";
      std::u32string output = U"";
      std::u32string expected = U"�";
      from8(input.cbegin(), std::back_inserter(output));
      AssertThat(output, Equals(expected));
    });

    it("2.6 Can detect a bad start char", [&]() {
      unsigned char input[] = {0xFE};
      std::u32string output = U"";
      AssertThrows(UnicodeError, from8(input, std::back_inserter(output)));
      AssertThat(LastException<UnicodeError>().what(), Is().EqualTo("Bad utf-8 first char"));
    });

    it("2.7 Can detect a bad mid stream char with extra bit set", [&]() {
      // Good sequence would be: 110xxxxx 10xxxxxx
      // Set the wrong bit:      110xxxxx 11xxxxxx
      unsigned char input[] = {0xE0, 0xC0};
      std::u32string output = U"";
      AssertThrows(UnicodeError, from8(input, std::back_inserter(output)));
      AssertThat(LastException<UnicodeError>().what(), Is().EqualTo("Bad utf-8 char"));
    });

    it("2.8 Can detect a bad mid stream char with extra bit set", [&]() {
      // Good sequence would be: 110xxxxx 10xxxxxx
      // Don't set the top bit:  110xxxxx 00xxxxxx
      unsigned char input[] = {0xE0, 0x00};
      std::u32string output = U"";
      AssertThrows(UnicodeError, from8(input, std::back_inserter(output)));
      AssertThat(LastException<UnicodeError>().what(), Is().EqualTo("Bad utf-8 char"));
    });

  });
});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }
