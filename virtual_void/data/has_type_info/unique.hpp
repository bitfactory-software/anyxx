#pragma once

#include <virtual_void/data/unique_trait.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/data/has_type_info/meta.hpp>

namespace virtual_void::data::has_type_info {
using unique = data::unique<meta>;
}

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_type_info::unique>
    : unique_trait<data::has_type_info::meta> {};

}  // namespace virtual_void

namespace virtual_void::data::has_type_info {
template <typename T>
using typed_unique = virtual_typed<T, unique>;
static_assert(is_virtual_void<unique>);
static_assert(is_virtual_typed<typed_unique<int>>);
}  // namespace virtual_void::data::has_type_info