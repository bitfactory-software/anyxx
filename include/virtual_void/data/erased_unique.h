#pragma once

#include <memory>

namespace virtual_void::data {

template <typename BASE>
using data_deleter = void (*)(BASE*);
template <typename BASE>
using erased_unique = std::unique_ptr<BASE, data_deleter<BASE>>;

template <typename BASE, typename T, typename... ARGS>
auto make_erased_unique(ARGS&&... args) {
  auto deleter = +[](BASE* meta) { delete static_cast<T*>(meta); };
  return erased_unique<BASE>(new T(std::forward<ARGS>(args)...), deleter);
}

}  // namespace virtual_void::data
