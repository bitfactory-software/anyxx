#pragma once

#include "../holder.h"

namespace virtual_void::erased::data::has_no_meta {

struct has_no_meta {
  const auto* get_meta() const { return this; }
  template <typename T>
  has_no_meta(std::in_place_type_t<T>) {}
  has_no_meta() = default;
  type_info_ptr type_info() const { return {}; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_no_meta() = default;
};

using with_no_meta = base<has_no_meta>;

}  // namespace virtual_void::erased::data