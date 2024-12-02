#pragma once

#include <virtual_void/data/erased_unique_ptr.hpp>

namespace virtual_void::data {

template <typename T, typename... ARGS>
auto make_unique_decorated_data(ARGS&&... args) {
  auto deleter = +[](typename T::base_t* meta) { delete static_cast<T*>(meta); };
  return erased_unique_ptr<typename T::base_t>(new T(std::in_place, std::forward<ARGS>(args)...), deleter);
}

}  // namespace virtual_void::data