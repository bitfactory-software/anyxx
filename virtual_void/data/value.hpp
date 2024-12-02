#pragma once

#include <virtual_void/data/erased_value.hpp>
#include <virtual_void/data/decorated_data.hpp>

namespace virtual_void::data {
template <typename META>
using value = erased_value<decoration_base<META>>;
}  // namespace virtual_void::data
