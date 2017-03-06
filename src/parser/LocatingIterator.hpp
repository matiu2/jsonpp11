/* To make an iteratooTo make an iterator*t An iterator wrapper that records a location in column and row coordinates */
#pragma once

#include "../utils.hpp"

namespace json {

/* I didn't use boost::iterator_adaptor because it had doesn't work with ifstream_iterator.
 * DIY Iterator based on the chart: http://www.cplusplus.com/reference/iterator/
 *
 * # All Categories
 *  1. Copy Constructable, assignable and deconstructible 
 *  2. Can be incremented
 * # Input
 *  3. == and !=
 *  4. *a and a->m
 */

template <typename Iter, typename traits = std::iterator_traits<Iter>>
struct LocatingIterator : public Iter  {

  using difference_type = typename traits::difference_type;
  using value_type = typename traits::value_type;
  using pointer = typename traits::pointer;
  using reference = typename traits::reference;
  using iterator_category = typename traits::iterator_category;

  int row = 1;
  int col = 1;

  // 1. Copy constructible and copy assignable

  LocatingIterator() : Iter() {}
  LocatingIterator(const Iter &iter) : Iter(iter) {}
  LocatingIterator(const LocatingIterator &other)
      : Iter(static_cast<Iter>(other)), row(other.row), col(other.col)  {}

  LocatingIterator& operator=(const LocatingIterator& other) {
    Iter& base(*this);
    const Iter& otherBase(other);
    base = otherBase;
    row = other.row;
    col = other.col;
    return *this;
  }

  // 2. Can be incremented
  
  void inline advance() {
    if (**this == '\n') {
      ++row;
      col = 1;
    } else {
      ++col;
    }
  }

  inline LocatingIterator operator ++(int) {
    LocatingIterator result(*this);
    Iter& base(*this);
    ++base;
    advance();
    return result;
  }

  inline LocatingIterator &operator++() {
    Iter& base(*this);
    base++;
    advance();
    return *this;
  }

  // 3. input iterator - equality and inequality

  bool inline operator==(const LocatingIterator &other) const {
    const Iter& base(*this);
    const Iter& otherBase(other);
    return base == otherBase;
  }

  bool inline operator!=(const LocatingIterator &other) const {
    const Iter& base(*this);
    const Iter& otherBase(other);
    return base != otherBase;
  }

  // 4. *a and a->m (already provided by base class)
  
};

template <typename Iter>
LocatingIterator<Iter> makeLocating(Iter iter) {
  return LocatingIterator<Iter>(iter);
}

}
