/// Utility function and types used by the parser components
#pragma once

#include <iterator>

namespace json {

// Type function shortcut for std::enable_if
template <bool B, typename T = void>
using enable_if = typename std::enable_if<B, T>::type;

// Shortcut for std::remove_pointer
template <typename T>
using remove_pointer = typename std::remove_pointer<T>::type;

// Shortcut for std::is_base_of
template <typename T, typename Y> constexpr bool is_base_of() {
  return std::is_base_of<T, Y>::value;
}

// Shortcut for std::is_lvalue_reference
template <typename T> constexpr bool is_lvalue_reference() {
  return std::is_lvalue_reference<T>::value;
}

// Shortcut for std::is_assignable
template <typename T, typename A> constexpr bool is_assignable() {
  return std::is_assignable<T, A>::value;
}

// Shortcut for std::is_integral
template <typename T> constexpr bool is_integral() {
  return std::is_integral<T>::value;
}

// Shortcut for std::is_const
template <typename T> constexpr bool is_const() {
  return std::is_const<T>::value;
}

// Shortcut for std::is_copy_assignable
template <typename T> constexpr bool is_copy_assignable() {
  return std::is_copy_assignable<T>::value;
}

/// Returns true if T is a forward iterator (can move forward)
template <typename T, typename Traits = std::iterator_traits<T>>
constexpr bool is_forward_iterator() {
  return std::is_base_of<std::forward_iterator_tag,
                         typename Traits::iterator_category>::value;
}

/// Returns true if Traits is an input iterator (can give us stuff to read )
template <typename T, typename Traits = std::iterator_traits<T>>
constexpr bool is_input_iterator() {
  return std::is_base_of<std::input_iterator_tag,
                         typename Traits::iterator_category>::value;
}

/// Returns true if Traits is an output iterator (can give us stuff to read )
template <typename T, typename Traits = std::iterator_traits<T>>
constexpr bool is_output_iterator() {
  return std::is_base_of<std::output_iterator_tag,
                         typename Traits::iterator_category>::value;
}

/// Returns true if Traits is a random access iterator (can give us stuff to
/// read )
template <typename T, typename Traits = std::iterator_traits<T>>
constexpr bool is_random_access_iterator() {
  return is_base_of<std::random_access_iterator_tag,
                    typename Traits::iterator_category>();
}

/// Returns true if T and Y are the same type
template <typename T, typename Y> constexpr bool is_same() {
  return std::is_same<T, Y>::value;
}

/// Shortcut for std::add_const
template <typename T>
using add_const = typename std::add_const<T>::type;

using std::declval;

// Means that parameter substitution when choosing a function failed
struct substitution_failed {};
// A type function, that has ::value == true unless passed 'substitution_failed'
template <typename T> struct check_substitution : std::true_type {};
template <> struct check_substitution<substitution_failed> : std::false_type {};

/// If we can compile this function, the iterator has location information
template <typename T>
enable_if<is_same<decltype(declval<T>().row), int>() &&
          is_same<decltype(declval<T>().col), int>()>
location(T iter);

substitution_failed location(...);

template <typename T>
struct check_location {
  using type = decltype(location(declval<T>()));
};

/// Shortcut type funcition to get 'check_location::type'
template <typename T>
struct do_has_location : check_substitution<typename check_location<T>::type> {};

/// A compile time function that returns 'true' if T has .row and .col
/// attributes that can be ints
template <typename T> constexpr bool has_location() {
  return do_has_location<T>::value;
}


/// An algoritm for checking if two series are equal, available in c++14, but we want it now :)
template <typename Iterator1, typename Iterator2,
          typename Iterator1Traits = std::iterator_traits<Iterator1>,
          typename Iterator2Traits = std::iterator_traits<Iterator2>>
bool equal(Iterator1 first_begin, Iterator1 first_end, Iterator2 second_begin,
           Iterator2 second_end) {
  static_assert(is_input_iterator<Iterator1Traits>(), "We need to be able to read from the input");
  static_assert(is_input_iterator<Iterator2Traits>(), "We need to be able to read from the comparison input");
  while ((first_begin != first_end) && (second_begin != second_end))
    if (*first_begin++ != *second_begin++)
      return false;
  return (first_begin == first_end) && (second_begin == second_end);
}

/// A compliler error (incomplete type). Useful for printing type of T. To use:
/// PRINT_TYPE<some_unknown_type> x;
template <typename T>
struct PRINT_TYPE; // If you try to use me

/// Shows a number at compile time (in an error message);
/// To use: SHOW_NUMBER<some_unknown_number> x;
template <int n>
struct SHOW_NUMBER;

template <typename T>
struct show_size_of {
  SHOW_NUMBER<sizeof(T)> x;
};

/**
* @brief Subtracts amount from iter
*
* @tparam T Iterator type
* @param iter iterator to subtract from
* @param amount amount to subtract
*/
template <typename T, typename Y>
enable_if<is_random_access_iterator<T>()>
subtract_from_iterator(T& iter, Y amount) {
  iter -= amount;
}

/**
* @brief Subtracts amount from iter
*
* @tparam T Iterator type
* @param iter iterator to subtract from
* @param amount amount to subtract
*/
template <typename T, typename Y>
void subtract_from_iterator(T& iter, Y amount) {
  for(int i=0; i<amount; ++i)
    --iter;
}

/**
* @brief Adds amount to iter
*
* @tparam T Iterator type
* @param iter iterator to add to
* @param amount amount to add
*/
template <typename T, typename Y>
enable_if<is_random_access_iterator<T>()>
add_to_iterator(T& iter, Y amount) {
  iter += amount;
}

/**
* @brief Adds amount to iter
*
* @tparam T Iterator type
* @param iter iterator to add to
* @param amount amount to add
*/
template <typename T, typename Y>
void add_to_iterator(T& iter, Y amount) {
  for(int i=0; i<amount; ++i)
    ++iter;
}

template <typename T, typename Traits = std::iterator_traits<T>>
enable_if<is_random_access_iterator<T>(), typename Traits::difference_type>
iterator_difference(T a, T b) {
  return b - a;
}

template <typename T, typename Traits = std::iterator_traits<T>>
enable_if<!is_random_access_iterator<T>(), typename Traits::difference_type>
iterator_difference(T a, T b) {
  typename Traits::difference_type result = 0;
  while (a != b) {
    ++a;
    ++result;
  }
  return result;
}
}
