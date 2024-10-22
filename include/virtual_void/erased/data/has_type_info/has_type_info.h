#pragma once

#include "../holder.h"

namespace virtual_void::erased::data {

struct has_type_info {
  const has_type_info* meta() const { return this; }
  std::type_info const* type_info_;
  template <typename T>
  has_type_info(std::in_place_type_t<T>)
      : type_info_(&typeid(std::decay_t<T>)) {}
  type_info_ptr type_info() const { return type_info_; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_type_info() = default;
};

using with_type_info = base<has_type_info>;

}  // namespace virtual_void::erased::data