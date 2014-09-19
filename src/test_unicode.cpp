/// Tests the conversion of unicode

#include <bandit/bandit.h>
#include <string>

#include "unicode.hpp"

using namespace bandit;
using namespace json;

go_bandit([]() {

  describe("utf-16 Reader", [&]() {

    it("1.0 Can read an empty string", [&]() {
      std::u16string input = u"";
      std::u32string output = U"";
      std::u32string expected = U"";
      from16(input.cbegin(), std::back_inserter(output));
      AssertThat(output, Equals(expected));
    });

  });

});

int main(int argc, char *argv[]) { return bandit::run(argc, argv); }