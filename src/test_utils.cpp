#include <boost/hana.hpp>
#include <boost/hana/ext/std/integral_constant.hpp>
#include <boost/hana/traits.hpp>

#include "utils.hpp"

#include <fstream>
#include <iterator>
#include <string>
#include <type_traits>

int main(int, char **) {
  // Test ifstream iterator - should be forward iterator
  std::istream_iterator<char> f;
  using namespace json;
  BOOST_HANA_CONSTANT_ASSERT(is_input_iterator(f));
  BOOST_HANA_CONSTANT_ASSERT(!is_forward_iterator(f));
  BOOST_HANA_CONSTANT_ASSERT(!is_bidirectional_iterator(f));
  BOOST_HANA_CONSTANT_ASSERT(!is_random_access_iterator(f));
  // Test std::string iterator
  std::string::iterator i;
  BOOST_HANA_CONSTANT_ASSERT(is_input_iterator(i));
  BOOST_HANA_CONSTANT_ASSERT(is_forward_iterator(i));
  BOOST_HANA_CONSTANT_ASSERT(is_bidirectional_iterator(i));
  BOOST_HANA_CONSTANT_ASSERT(is_random_access_iterator(i));
  // Test output iterator
  std::string s;
  BOOST_HANA_CONSTANT_ASSERT(is_output_iterator(std::back_inserter(s)));
  BOOST_HANA_CONSTANT_ASSERT(!is_input_iterator(std::back_inserter(s)));
  BOOST_HANA_CONSTANT_ASSERT(!is_forward_iterator(std::back_inserter(s)));
  BOOST_HANA_CONSTANT_ASSERT(!is_bidirectional_iterator(std::back_inserter(s)));
  BOOST_HANA_CONSTANT_ASSERT(!is_random_access_iterator(std::back_inserter(s)));
  return 0;
}
