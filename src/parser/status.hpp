/// A small utility class to help remember the status of the parser
#pragma once

#include "error.hpp"
#include "../utils.hpp"

namespace json {

template <typename Iterator,
          typename Iterator_traits = std::iterator_traits<Iterator>>
struct Status {

  using iterator = Iterator;
  using iterator_traits = Iterator_traits;

  static_assert(is_input_iterator<iterator_traits>(),
                "The iterator must be an input iterator");
  static_assert(is_same<typename iterator_traits::value_type, char>(),
                "We only work on char input");

  iterator p;
  const iterator pe;
  ErrorThrower<iterator> onError = throwError<iterator>;

  Status(iterator p, iterator pe, ErrorThrower<iterator> onError = throwError<iterator>)
  : p(p), pe(pe), onError(onError) {}
};

template <typename iterator,
          typename iterator_traits = std::iterator_traits<iterator>>
Status<iterator, iterator_traits>
make_status(iterator p, iterator pe,
            ErrorThrower<iterator> onError = throwError<iterator>) {
  return Status<iterator, iterator_traits>(p, pe, onError);
}

/// Check if something is a valid status
template <typename T>
enable_if<
    is_input_iterator<typename T::iterator_traits>() &&
        is_copy_assignable<typename T::iterator>() &&
        is_same<typename T::iterator_traits::value_type, char>() &&
        is_same<decltype(declval<T>().p), typename T::iterator>() &&
        is_same<decltype(declval<T>().pe), add_const<typename T::iterator>>(),
    T> _status(T);
substitution_failed _status(...);
template <typename T> struct check_status {
  using type = decltype(_status(declval<T>()));
};
template <typename T>
struct do_is_valid_status : check_substitution<typename check_status<T>::type> {
};
template <typename T> constexpr bool is_valid_status() {
  return do_is_valid_status<T>::value;
}

// Util functions that depend on status

/**
 * Compare's what we're seeing in the JSON to a zero terminated static string
 *
 * @tparam iterator an Input Iterator to chars
 * @tparam iterator_traits iterator traits for 'iterator'
 * @param p Points to the first character of the JSON input
 * @param pe Points to one past the end of the JSON input
 * @param onError A function to call when we encounter an error
 * @param returns the new iterator position
 */
template <typename Status>
inline void
checkStaticString(Status& status, const char *expected) {
  static_assert(is_valid_status<Status>(), "The status object must have "
                                           "iterators p, pe, and an error "
                                           "thrower function; onError");
  while (*expected)
    if ((*expected++) != (*status.p++))
      status.onError(std::string("Static String '") + expected +
                     "' doesn't match", status.p);
}

}
