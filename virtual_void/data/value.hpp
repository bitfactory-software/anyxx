#pragma once

#include "erased_value.hpp"
#include "decorated_data.hpp"

namespace virtual_void::data {
template <typename META>
using value = erased_value<decoration_base<META>>;
}  // namespace virtual_void::data
