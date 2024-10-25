#pragma once

#include "has_m_table.h"
#include "../unique_trait.h"
#include "../../virtual_void.h"

namespace virtual_void::m_table {
using unique_data_ptr = erased::data::unique_ptr<erased::data::with_m_table>;
}

namespace virtual_void::erased {
template <>
struct data_trait<m_table::unique_data_ptr>
    : unique_trait<data::has_m_table> {};

}  // namespace virtual_void::erased

namespace virtual_void::m_table {

using unique = erased::virtual_void<unique_data_ptr>;
template <typename T>
using typed_unique = erased::virtual_typed<T, unique_data_ptr>;

static_assert(erased::is_virtual_void<typed_unique<int> >);
static_assert(erased::is_virtual_void<unique>);

}  // namespace virtual_void::m_table