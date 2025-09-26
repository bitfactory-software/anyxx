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
static_assert(has_erased_data_trait<unique_ptr>);
static_assert(is_erased_data<unique_ptr>);
}  // namespace virtual_void::data::has_no_meta