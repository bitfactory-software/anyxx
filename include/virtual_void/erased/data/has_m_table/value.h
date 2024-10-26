#pragma once

#include <stdexcept>
#include <type_traits>

#include "../value_trait.h"
#include "meta.h"

namespace virtual_void::erased::data::has_m_table {
using value_data_ptr = value_ptr<with_m_table>;
}

namespace virtual_void::erased {
using namespace virtual_void;
template <>
struct data_trait<data::has_m_table::value_data_ptr>
    : value_trait<data::has_m_table::meta> {};
}  // namespace virtual_void::erased

namespace virtual_void::erased::data::has_m_table {
using value = erased::virtual_void<value_data_ptr>;
template <typename T>
using typed_value = erased::virtual_typed<T, value_data_ptr>;
static_assert(erased::is_virtual_void<value>);
static_assert(erased::is_virtual_void<typed_value<int>>);
}  // namespace virtual_void::erased::data::has_m_table