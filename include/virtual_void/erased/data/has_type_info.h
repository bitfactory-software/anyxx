#pragma once

#include "../../forward.h"

namespace virtual_void::erased::data {

struct has_type_info {
  std::type_info const* type_info_;
  template <typename T>
  has_type_info(std::in_place_type_t<T>)
      : type_info_(&typeid(std::decay_t<T>)) {}
  type_info_ptr type_info() const { return type_info_; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_type_info() = default;
};

}  // namespace virtual_void::erased::data