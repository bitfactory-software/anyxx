#pragma once

#include <memory>

namespace virtual_void::data {

template <typename META_DATA>
using data_deleter = void (*)(META_DATA*);

template <typename META_DATA>
using unique_ptr = std::unique_ptr<META_DATA, data_deleter<META_DATA>>;

template <typename T, typename... ARGS>
auto make_unique_decorated_data(ARGS&&... args) {
  using base_t = T::base_t;
  auto deleter = +[](base_t* meta) { delete static_cast<T*>(meta); };
  return unique_ptr<base_t>(
      new T(std::in_place, std::forward<ARGS>(args)...), deleter);
}

}  // namespace virtual_void::data