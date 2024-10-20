#pragma once

#include "typed.h"

namespace virtual_void::erased::data {

template <typename META_DATA>
using data_deleter = void (*)(META_DATA*);

template <typename META_DATA>
using unique_ptr = std::unique_ptr<META_DATA, data_deleter<META_DATA>>;

template <typename T, typename... ARGS>
auto make_unique(ARGS&&... args) {
  using meta_data_t = T::meta_data_t;
  auto deleter = +[](meta_data_t* meta) { delete static_cast<T*>(meta); };
  return unique_ptr<meta_data_t>(
      new T(std::in_place, std::forward<ARGS>(args)...), deleter);
}

}  // namespace virtual_void::erased::data