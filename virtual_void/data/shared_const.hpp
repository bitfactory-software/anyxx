#pragma once

#include <memory>

#include <virtual_void/virtual_void.hpp>
#include <virtual_void/data/decorated_data.hpp>
#include <virtual_void/data/shared_const_trait.hpp>

namespace virtual_void::data {
using shared_const = std::shared_ptr<void const>;
}  // namespace virtual_void::data

namespace virtual_void {
template <>
struct erased_data_trait<data::shared_const>
    : shared_const_trait {};
}  // namespace virtual_void

namespace virtual_void::data {
static_assert(is_erased_data<shared_const>);
}