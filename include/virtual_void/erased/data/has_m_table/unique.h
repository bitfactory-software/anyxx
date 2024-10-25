#pragma once

#include "meta.h"
#include "../unique_trait.h"

namespace virtual_void::erased::data::has_m_table {
using unique_data_ptr = unique_ptr<with_m_table>;
}

namespace virtual_void::erased {
template <>
struct data_trait<data::has_m_table::unique_data_ptr>
    : unique_trait<data::has_m_table::has_m_table> {};

}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_m_table {

using unique = erased::virtual_void<unique_data_ptr>;
template <typename T>
using typed_unique = virtual_typed<T, unique_data_ptr>;

static_assert(erased::is_virtual_void<typed_unique<int> >);
static_assert(erased::is_virtual_void<unique>);

}  // namespace virtual_void::m_table