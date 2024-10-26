#pragma once

#include "../holder.h"

namespace virtual_void::erased::data::has_no_meta {

struct meta {
  const auto* get_meta() const { return this; }
  template <typename T>
  meta(std::in_place_type_t<T>) {}
  meta() = default;
  type_info_ptr type_info() const { return {}; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~meta() = default;
};

using with_no_meta = base<meta>;

}  // namespace virtual_void::erased::data