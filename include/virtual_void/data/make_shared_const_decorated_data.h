#pragma once

#include "memory"

namespace virtual_void::data {

template <typename META_DATA>
using shared_const_ptr = std::shared_ptr<META_DATA const>;

template <typename T, typename... ARGS>
auto make_shared_const_decorated_data(ARGS&&... args) {
  return std::make_shared<T const>(std::in_place, std::forward<ARGS>(args)...);
}

}  // namespace virtual_void::data