#pragma once

#include <virtual_void/data/shared_const_trait.hpp>
#include <virtual_void/data/shared_const.hpp>


namespace virtual_void::data::has_no_meta {
using shared_const = data::shared_const;
};

namespace virtual_void {
template <>
struct erased_data_trait<data::has_no_meta::shared_const>
    : shared_const_trait {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
static_assert(is_erased_data<shared_const>);
}  // namespace virtual_void::data::has_no_meta