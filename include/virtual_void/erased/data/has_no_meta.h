#pragma once

#include "../../forward.h"

namespace virtual_void::erased::data {

struct has_no_meta {
  template <typename T>
  has_no_meta(std::in_place_type_t<T>) {}
  type_info_ptr type_info() const { return {}; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_no_meta() = default;
};

}  // namespace virtual_void::erased::data