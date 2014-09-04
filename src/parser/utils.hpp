/// Utility function and types used by the parser components
#pragma once

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

// Means that parameter substitution when choosing a function failed
struct substitution_failed {};
// A type function, that has ::value == true unless passed 'substitution_failed'
template <typename T> struct check_substitution : std::true_type {};
template <> struct check_substitution<substitution_failed> : std::false_type {};

/// If we can compile this function, the iterator has location information
template <typename T>
enable_if<is_same<decltype(std::declval<T>().row), int>() &&
          is_same<decltype(std::declval<T>().col), int>()>
location(T iter);

substitution_failed location(...);

template <typename T>
struct check_location {
  using type = decltype(location(std::declval<T>()));
};

/// Shortcut type funcition to get 'check_location::type'
template <typename T>
struct do_has_location : check_substitution<typename check_location<T>::type> {};

/// A compile time function that returns 'true' if T has .row and .col
/// attributes that can be ints
template <typename T> constexpr bool has_location() {
  return do_has_location<T>::value;
}
}
