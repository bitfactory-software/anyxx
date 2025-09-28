#pragma once

#include <virtual_void/data/erased_unique_ptr.hpp>

namespace virtual_void::data {

template <typename T>
auto erased_delete() {
  return +[](void* data) { delete static_cast<T*>(data); };
}

template <typename T>
auto move_to_unique_void(std::unique_ptr<T> p) {
  return erased_unique_ptr<void>(p.release(), erased_delete<T>());
}

template <typename T, typename... ARGS>
auto make_unique(ARGS&&... args) {
  return move_to_unique_void(std::make_unique<T>(std::forward<ARGS>(args)...));
}

}  // namespace virtual_void::data