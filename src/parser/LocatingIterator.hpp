/** An iterator wrapper that records a location in column and row coordinates */
#pragma once

namespace json {

template <typename T> struct LocatingIterator : T {
  using original_type = T;
  int row = 1;
  int col = 1;
  inline LocatingIterator<T> &operator++() {
    if (**this == '\n') {
      ++row;
      col = 1;
    } else {
      ++col;
    }
    T::operator++();
    return *this;
  }
  inline LocatingIterator<T> operator++(int) {
    LocatingIterator<T> result = *this;
    operator ++();
    return result;
  }
  LocatingIterator(T normalIterator) : T(normalIterator) {}
};

template <typename T>
LocatingIterator<T> makeLocating(T iter) {
  return LocatingIterator<T>(iter);
}

}
