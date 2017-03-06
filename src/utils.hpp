/// Utility function and types used by the parser components
#pragma once

#include <iterator>
#include <boost/hana.hpp>
#include <boost/hana/traits.hpp>
#include <boost/hana/ext/std/integral_constant.hpp>

namespace hana = boost::hana;

namespace json {

auto is_default_constructible =
    hana::is_valid([](auto &&x) -> decltype(declval(decltype(x)())) {});
auto is_derefencable = hana::is_valid([](auto &&x) -> decltype(*x) {});
auto is_incrementable = hana::is_valid([](auto &&x) -> decltype(++x) {});
auto is_decrementable = hana::is_valid([](auto &&x) -> decltype(x--) {});
auto supports_equality = hana::is_valid([](auto &&x) -> decltype(x == x) {});
auto supports_inequality = hana::is_valid([](auto &&x) -> decltype(x != x) {});
auto supports_bi_addition = hana::is_valid(
    [](auto &&x) -> std::tuple<decltype(x + 1), decltype(1 + x)> {});
auto supports_int_subtraction =
    hana::is_valid([](auto &&x) -> std::tuple<decltype(x - 1)> {});
auto supports_subtraction =
    hana::is_valid([](auto &&x) -> std::tuple<decltype(x - x)> {});
auto supports_comparison = hana::is_valid(
    [](auto &&x) -> std::tuple<decltype(x < x), decltype(x > x),
                               decltype(x <= x), decltype(x >= x)> {});
auto supports_offset = hana::is_valid([](auto &&x) -> decltype(x[1]) {});

/// Returns true if T is an input iterator - can advance but not copy.
auto is_input_iterator = [](auto &&x) {
  auto z = hana::type_c<decltype(x)>;
  return hana::traits::is_copy_constructible(z) && is_incrementable(x) &&
         supports_equality(x) && supports_inequality(x) && is_derefencable(x);
};

/// Returns true if T is a forward iterator (can move forward) and dereference
/// an lvalue
auto is_forward_iterator = [](auto x) {
  using T = decltype(x);
  using Traits = std::iterator_traits<T>;
  auto z = hana::type_c<T>;
  auto dereferenced = hana::type_c<decltype(*x)>;
  auto tag = hana::type_c<typename Traits::iterator_category>;
  auto acceptable_tags =
      hana::tuple_t<std::forward_iterator_tag, std::bidirectional_iterator_tag,
                    std::random_access_iterator_tag>;
  auto matchesTag = [tag](auto aTag) { return aTag == tag; };
  return is_input_iterator(x) && hana::traits::is_default_constructible(z) &&
         hana::traits::is_copy_assignable(z) &&
         hana::traits::is_lvalue_reference(dereferenced) &&
         // It also checks if the iterator is multipass, but I don't know how to
         // emulate that at compile time, so I resort to tags here
         hana::any_of(acceptable_tags, matchesTag);
};

auto is_bidirectional_iterator = [](auto &&x) {
  return is_forward_iterator(x) && is_decrementable(x);
};

auto is_random_access_iterator = [](auto &&x) {
  return is_bidirectional_iterator(x) && supports_bi_addition(x) &&
         supports_int_subtraction(x) && supports_subtraction(x) &&
         supports_comparison(x) && supports_offset(x);
};

auto is_output_iterator = [](auto &&x) {
  auto t1 = hana::type_c<decltype(*x)>;
  auto t2 = hana::type_c<decltype(*x++)>;
  return hana::traits::is_assignable(t1, t1) &&
         hana::traits::is_assignable(t2, t1);
};

}
