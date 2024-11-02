#pragma once

#include "../shared_const_ptr_trait.h"
#include "meta.h"

namespace virtual_void::data::has_no_meta {
using shared_const_ptr = data::shared_const_ptr<meta>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_no_meta::shared_const_ptr>
    : shared_const_ptr_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
static_assert(has_virtual_void_trait<shared_const_ptr>);
template <typename T>
using typed_shared_const_ptr = virtual_typed<T const, shared_const_ptr>;
static_assert(is_virtual_void<shared_const_ptr>);
static_assert(is_virtual_typed<typed_shared_const_ptr<int>>);
}  // namespace virtual_void::data::has_no_meta