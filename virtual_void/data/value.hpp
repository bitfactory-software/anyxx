#pragma once

#include <virtual_void/data/erased_value.hpp>
#include <virtual_void/data/decorated_data.hpp>
#include <virtual_void/data/value_trait.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

using value = erased_value<decoration_base>;
}  // namespace virtual_void::data

namespace virtual_void {
template <>
struct erased_data_trait<data::value>
    : value_trait {};
}  // namespace virtual_void

namespace virtual_void::data {
static_assert(is_erased_data<value>);
}  // namespace virtual_void::data::has_no_meta