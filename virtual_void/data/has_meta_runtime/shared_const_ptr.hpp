#pragma once

#include <virtual_void/data/shared_const_ptr_trait.hpp>
#include <virtual_void/data/has_meta_runtime/meta.hpp>

namespace virtual_void::data::has_meta_runtime{
using shared_const_ptr = data::shared_const_ptr<meta>;
}

namespace virtual_void {
template <>
struct erased_data_trait<data::has_meta_runtime::shared_const_ptr>
    : shared_const_ptr_trait<data::has_meta_runtime::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_meta_runtime {
static_assert(has_erased_data_trait<shared_const_ptr>);
static_assert(is_erased_data<shared_const_ptr>);
}  // namespace virtual_void::data::has_meta_runtime