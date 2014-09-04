/// A small utility class to help remember the status of the parser
#pragma once

#include "error.hpp"

namespace json {

template <typename iterator,
          typename iterator_traits = std::iterator_traits<iterator>>
struct Status {

  static_assert(is_input_iterator<iterator_traits>(),
                "The iterator must be an input iterator");
  static_assert(is_copy_assignable<iterator>(),
                "The iterator must support copying by assignment");
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

}
