#pragma once

#include "erased_unique.h"

namespace virtual_void::data {

template <typename T, typename... ARGS>
auto make_unique_decorated_data(ARGS&&... args) {
  return make_erased_unique<typename T::base_t, T>(std::in_place,
                                          std::forward<ARGS>(args)...);
}

}  // namespace virtual_void::data