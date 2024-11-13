#pragma once

#include "../shared_const_trait.h"
#include "../shared_const.h"
#include "meta.h"

namespace virtual_void::data::has_type_info {
using shared_const = data::shared_const<meta>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_type_info::shared_const>
    : shared_const_trait<data::has_type_info::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_type_info {
static_assert(has_virtual_void_trait<shared_const>);
template <typename T>
using typed_shared_const = virtual_typed<T const, shared_const>;
static_assert(is_virtual_void<shared_const>);
static_assert(is_virtual_typed<typed_shared_const<int>>);
}  // namespace virtual_void::data::has_type_info