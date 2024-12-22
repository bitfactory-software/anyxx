#pragma once

#include <virtual_void/data/has_i_table/meta.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/data/unique_trait.hpp>

namespace virtual_void::data::has_i_table {
using unique = data::unique<meta>;
inline auto unique_nullptr() { return data::unique_nullptr<meta>(); }
}  // namespace virtual_void::data::has_i_table

namespace virtual_void {
template <>
struct virtual_void_trait<data::has_i_table::unique>
    : unique_trait<data::has_i_table::meta> {};

}  // namespace virtual_void

namespace virtual_void::data::has_i_table {
template <typename T>
using typed_unique = virtual_typed<T, unique>;
static_assert(is_virtual_void<unique>);
static_assert(is_virtual_typed<typed_unique<int>>);
}  // namespace virtual_void::data::has_i_table