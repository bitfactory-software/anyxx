#pragma once

#include <virtual_void/data/value_trait.hpp>
#include <virtual_void/data/value.hpp>
#include <virtual_void/data/has_type_info/meta.hpp>

namespace virtual_void::data::has_type_info {
using value = data::value<meta>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_type_info::value>
    : value_trait<data::has_type_info::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_type_info {
template <typename T>
using typed_value = virtual_typed<T, value>;
static_assert(is_virtual_void<value>);
static_assert(is_virtual_typed<typed_value<int>>);
}  // namespace virtual_void::data::has_type_info