/// A small utility class to help remember the status of the parser
#pragma once

#include "error.hpp"

namespace json {

auto is_comparable_to_char =
    hana::is_valid([](auto &&x) -> decltype(x == 'c') {});

template <typename Iterator,
          typename Iterator_traits = std::iterator_traits<Iterator>>
struct Status {

  using iterator = Iterator;
  using iterator_traits = Iterator_traits;

  iterator p;
  iterator pe;
  ErrorThrower<iterator> _onError = throwError<iterator>;

  Status(iterator p, iterator pe,
         ErrorThrower<iterator> onError = throwError<iterator>)
      : p(p), pe(pe), _onError(onError) {
    BOOST_HANA_CONSTANT_ASSERT(is_input_iterator(p));
    BOOST_HANA_CONSTANT_ASSERT(is_comparable_to_char(*p));
  }

  Status& operator =(Status& other) {
    p = other.p;
    pe = other.pe;
    _onError = other._onError;
    return *this;
  }

  void onError(const std::string& msg) const {
    if (_onError)
      _onError(msg, p);
  }

  Status<Iterator, Iterator_traits> copy() const {
    Status<Iterator, Iterator_traits> result = *this;
    return result;
  }

};

template <typename iterator,
          typename iterator_traits = std::iterator_traits<iterator>>
Status<iterator, iterator_traits>
make_status(iterator p, iterator pe,
            ErrorThrower<iterator> onError = throwError<iterator>) {
  return Status<iterator, iterator_traits>(p, pe, onError);
}

/// Check if something is a valid status

auto is_valid_status = [](auto && status) {
  auto has_p_and_pe =
      hana::is_valid([](auto &&status) -> std::tuple<decltype(status.p),
                                                     decltype(status.pe)>{});
  return has_p_and_pe(status) && is_input_iterator(status.p) &&
         hana::traits::is_copy_assignable(hana::type_c<decltype(status.p)>) &&
         is_comparable_to_char(*status.p) &&
         is_comparable_to_char(*status.pe);
};

// Util functions that depend on status

/**
 * Compare's what we're seeing in the JSON to a zero terminated static string
 *
 * @tparam Status a reference Status instance
 * @param returns true if we found the expected string
 */
template <typename Status,
          typename Char = typename Status::iterator_traits::value_type>
inline bool
checkStaticString(Status& status, const Char *expected) {
  BOOST_HANA_CONSTANT_CHECK(is_valid_status(status));
  while (*expected)
    if ((*expected++) != (*status.p++))
      return false;
  return true;
}

/**
 * Compare's what we're seeing in the JSON to a zero terminated static string
 *
 * @tparam Status a reference Status instance
 * @param status the current parser status
 * @param expected the string that we are expecting to read
 * @param returns the new iterator position
 */
template <typename Status,
          typename Char = typename Status::iterator_traits::value_type>
inline void requireStaticString(Status &status, const Char *expected) {
  BOOST_HANA_CONSTANT_CHECK(is_valid_status(status));
  if (!checkStaticString(status, expected))
    status.onError(std::string("Static String '") + expected +
                   "' doesn't match");
}
}
