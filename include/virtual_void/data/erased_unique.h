#pragma once

#include "erased_unique_ptr.h"

namespace virtual_void::data {

template <typename BASE>
using erased_unique = erased_unique_ptr<BASE>;

template <typename BASE, typename T, typename... ARGS>
auto make_erased_unique(ARGS&&... args) {
  auto deleter = +[](BASE* meta) { delete static_cast<T*>(meta); };
  return erased_unique<BASE>(new T(std::forward<ARGS>(args)...), deleter);
}

}  // namespace virtual_void::data
