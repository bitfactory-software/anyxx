#pragma once

#include "erased_shared_const.hpp"

namespace virtual_void::data {

template <typename T, typename... ARGS>
auto make_shared_const_decorated_data(ARGS&&... args) {
  return make_erased_shared_const<typename T::base_t, T>(
      std::in_place, std::forward<ARGS>(args)...);
}

}  // namespace virtual_void::data