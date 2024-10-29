#pragma once

#include "meta.h"
#include "../unique_trait.h"

namespace virtual_void::erased::data::has_m_table {
using unique_DATA = unique_ptr<allocation_base<meta>>;
}

namespace virtual_void::erased {
template <>
struct virtual_void_trait<data::has_m_table::unique_DATA>
    : unique_trait<data::has_m_table::meta> {};

}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_m_table {
using unique = erased::virtual_void<unique_DATA>;
template <typename T>
using typed_unique = virtual_typed<T, unique_DATA>;
static_assert(erased::is_virtual_void<typed_unique<int> >);
static_assert(erased::is_virtual_void<unique>);
}  // namespace virtual_void::m_table