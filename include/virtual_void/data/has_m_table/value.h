#pragma once

#include <stdexcept>
#include <type_traits>

#include "../value_trait.h"
#include "meta.h"

namespace virtual_void::data::has_m_table {
using value = erased_value<decoration_base<meta>>;
}

namespace virtual_void {
using namespace virtual_void;
template <>
struct virtual_void_trait<data::has_m_table::value>
    : value_trait<data::has_m_table::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_m_table {
template <typename T>
using typed_value = virtual_typed<T, value>;
static_assert(is_virtual_void<value>);
static_assert(is_virtual_typed<typed_value<int>>);
}  // namespace virtual_void::data::has_m_table