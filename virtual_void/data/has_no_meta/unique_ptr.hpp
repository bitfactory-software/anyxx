#pragma once

#include <virtual_void/data/unique_ptr_trait.hpp>
#include <virtual_void/data/has_no_meta/meta.hpp>

namespace virtual_void::data::has_no_meta {
using unique_ptr = data::unique_ptr<meta>;
}

namespace virtual_void {
template <>
struct erased_data_trait<data::has_no_meta::unique_ptr>
    : unique_ptr_trait<data::has_no_meta::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
static_assert(has_virtual_void_trait<unique_ptr>);
template <typename T>
using typed_unique_ptr = virtual_typed<T const, unique_ptr>;
static_assert(is_virtual_void<unique_ptr>);
static_assert(is_virtual_typed<typed_unique_ptr<int>>);
}  // namespace virtual_void::data::has_no_meta