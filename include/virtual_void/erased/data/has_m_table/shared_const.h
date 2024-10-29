#pragma once

#include "../shared_const_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_m_table {
using shared_const_data = shared_const_ptr<allocation_base<meta>>;
}

namespace virtual_void::erased {
template <>
struct virtual_void_trait<erased::data::has_m_table::shared_const_data>
    : shared_const_trait<erased::data::has_m_table::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_m_table {
static_assert(has_virtual_void_trait<shared_const_data>);
using shared_const = virtual_void<shared_const_data>;
template <typename T>
using typed_shared_const = virtual_typed<T const, shared_const_data>;
static_assert(erased::is_virtual_void<shared_const>);
static_assert(erased::is_virtual_void<typed_shared_const<int>>);
}  // namespace virtual_void::erased::data::has_m_table