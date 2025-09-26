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
static_assert(is_erased_data<unique>);
}  // namespace virtual_void::data::has_meta_runtime