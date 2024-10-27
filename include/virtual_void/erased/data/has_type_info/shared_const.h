#pragma once

#include "../shared_const_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_type_info {
using shared_const_data_ptr = erased::data::shared_const_ptr<base<meta>>;
}

namespace virtual_void::erased {
template <>
struct virtual_void_trait<erased::data::has_type_info::shared_const_data_ptr>
    : shared_const_trait<erased::data::has_type_info::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_type_info {
static_assert(erased::is_data_pointer<shared_const_data_ptr>);
using shared_const = erased::virtual_void<shared_const_data_ptr>;
template <typename T>
using typed_shared_const =
    erased::virtual_typed<T const, shared_const_data_ptr>;
static_assert(erased::is_virtual_void<shared_const>);
static_assert(erased::is_virtual_void<typed_shared_const<int>>);
}  // namespace virtual_void::erased::data::has_type_info