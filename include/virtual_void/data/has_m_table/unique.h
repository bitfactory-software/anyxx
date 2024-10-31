#pragma once

#include "meta.h"
#include "../unique_trait.h"

namespace virtual_void::data::has_m_table {
using unique_data = unique_ptr<allocation_base<meta>>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_m_table::unique_data>
    : unique_trait<data::has_m_table::meta> {};

}  // namespace virtual_void

namespace virtual_void::data::has_m_table {
using unique = unique_data;
template <typename T>
using typed_unique = virtual_typed<T, unique_data>;
static_assert(is_virtual_void<unique>);
static_assert(is_virtual_typed<typed_unique<int> >);
}  // namespace virtual_void::m_table