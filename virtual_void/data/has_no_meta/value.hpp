#pragma once

#include <virtual_void/data/value_trait.hpp>
#include <virtual_void/data/value.hpp>
#include <virtual_void/virtual_void.hpp>


namespace virtual_void::data::has_no_meta {
using value = data::value;
}

namespace virtual_void {
template <>
struct erased_data_trait<data::has_no_meta::value>
    : value_trait {};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
static_assert(is_erased_data<value>);
}  // namespace virtual_void::data::has_no_meta