#pragma once

#include <virtual_void/data/value_trait.hpp>
#include <virtual_void/data/value.hpp>
#include <virtual_void/data/has_meta_runtime/meta.hpp>

namespace virtual_void::data::has_meta_runtime {
using value = data::value<meta>;
}

namespace virtual_void {
template <>
struct erased_data_trait<data::has_meta_runtime::value>
    : value_trait<data::has_meta_runtime::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_meta_runtime {
template <typename T>
using typed_value = virtual_typed<T, value>;
static_assert(is_erased_data<value>);
static_assert(is_virtual_typed<typed_value<int>>);
}  // namespace virtual_void::data::has_meta_runtime