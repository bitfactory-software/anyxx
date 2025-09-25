#pragma once

#include <virtual_void/data/has_meta_runtime/meta.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/data/unique_trait.hpp>

namespace virtual_void::data::has_meta_runtime {
using unique = data::unique<virtual_void::data::has_meta_runtime::meta>;
inline auto unique_nullptr() { return data::unique_nullptr<has_meta_runtime::meta>(); }
}  // namespace virtual_void::data::has_meta_runtime

namespace virtual_void {
template <>
struct erased_data_trait<data::has_meta_runtime::unique>
    : unique_trait<data::has_meta_runtime::meta> {};

}  // namespace virtual_void

namespace virtual_void::data::has_meta_runtime {
template <typename T>
using typed_unique = virtual_typed<T, unique>;
static_assert(is_virtual_void<unique>);
static_assert(is_virtual_typed<typed_unique<int>>);
}  // namespace virtual_void::data::has_meta_runtime