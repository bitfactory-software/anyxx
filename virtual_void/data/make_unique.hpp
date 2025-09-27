#pragma once

#include <virtual_void/data/erased_unique_ptr.hpp>

namespace virtual_void::data {

template <typename T, typename... ARGS>
auto make_unique(ARGS&&... args) {
  auto deleter = +[](void* data) { delete static_cast<T*>(data); };
  return erased_unique_ptr<void>(new T(std::forward<ARGS>(args)...), deleter);
}

}  // namespace virtual_void::data