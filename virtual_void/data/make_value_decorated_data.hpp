#pragma once

#include <virtual_void/data/erased_value.hpp>
#include <virtual_void/data/decorated_data.hpp>

namespace virtual_void::data {

template <typename T, typename... ARGS>
auto make_value_decorated_data(ARGS&&... args) {
  using base_t = T::base_t;
  static_assert(std::derived_from<T, base_t>);
  return make_erased_value<base_t, T>(std::in_place, std::forward<ARGS>(args)...);
}

}  // namespace virtual_void::data