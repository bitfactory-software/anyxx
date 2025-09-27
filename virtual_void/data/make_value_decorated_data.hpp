#pragma once

#include <virtual_void/data/erased_value.hpp>
#include <virtual_void/data/decorated_data.hpp>

namespace virtual_void::data {

template <typename T, typename... ARGS>
auto make_value_decorated_data(ARGS&&... args) {
  return make_erased_value<T>(std::forward<ARGS>(args)...);
}

}  // namespace virtual_void::data