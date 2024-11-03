#pragma once

#include "../unique_ptr_trait.h"
#include "meta.h"

namespace virtual_void::data::has_m_table {
using unique_ptr = data::unique_ptr<meta>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_m_table::unique_ptr>
    : unique_ptr_trait<data::has_m_table::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_m_table {
static_assert(has_virtual_void_trait<unique_ptr>);
template <typename T>
using typed_unique_ptr = virtual_typed<T const, unique_ptr>;
static_assert(is_virtual_void<unique_ptr>);
static_assert(is_virtual_typed<typed_unique_ptr<int>>);
}  // namespace virtual_void::data::has_m_table