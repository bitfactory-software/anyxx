#pragma once

#include <memory>

namespace virtual_void::data {

template <typename T, typename... ARGS>
std::shared_ptr<typename T::base_t const> make_shared_const_decorated_data(
    ARGS&&... args) {
  return std::make_shared<T const>(std::in_place, std::forward<ARGS>(args)...);
}

}  // namespace virtual_void::data