#pragma once

#include "../unique_ptr_trait.hpp"
#include "meta.hpp"

namespace virtual_void::data::has_type_info {
using unique_ptr = data::unique_ptr<meta>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_type_info::unique_ptr>
    : unique_ptr_trait<data::has_type_info::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_type_info {
static_assert(has_virtual_void_trait<unique_ptr>);
template <typename T>
using typed_unique_ptr = virtual_typed<T const, unique_ptr>;
static_assert(is_virtual_void<unique_ptr>);
static_assert(is_virtual_typed<typed_unique_ptr<int>>);
}  // namespace virtual_void::data::has_type_info