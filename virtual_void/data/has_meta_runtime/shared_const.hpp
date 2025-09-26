#pragma once

#include <virtual_void/data/shared_const_trait.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/has_meta_runtime/meta.hpp>

namespace virtual_void::data::has_meta_runtime {
using shared_const = data::shared_const<meta>;
}

namespace virtual_void {
template <>
struct erased_data_trait<data::has_meta_runtime::shared_const>
    : shared_const_trait<data::has_meta_runtime::meta> {};
}  // namespace virtual_void

namespace virtual_void::data::has_meta_runtime {
static_assert(has_erased_data_trait<shared_const>);
}  // namespace virtual_void::data::has_meta_runtime