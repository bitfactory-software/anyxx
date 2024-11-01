#pragma once

#include "../shared_const_trait.h"
#include "meta.h"

namespace virtual_void::data::has_type_info {
using shared_const_data = data::shared_const_ptr<decoration_base<meta>>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_type_info::shared_const_data>
    : shared_const_trait<data::has_type_info::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_type_info {
static_assert(has_virtual_void_trait<shared_const_data>);
using shared_const = shared_const_data;
template <typename T>
using typed_shared_const = virtual_typed<T const, shared_const_data>;
static_assert(is_virtual_void<shared_const>);
static_assert(is_virtual_typed<typed_shared_const<int>>);
}  // namespace virtual_void::data::has_type_info