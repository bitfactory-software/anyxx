#pragma once

#include <virtual_void/data/erased_unique_ptr.hpp>
#include <virtual_void/data/decorated_data.hpp>

namespace virtual_void::data {
template <typename META>
using unique = erased_unique_ptr<decoration_base<META>>;
}  // namespace virtual_void::data
