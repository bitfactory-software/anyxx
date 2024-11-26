#pragma once

#include "erased_unique.hpp"
#include "decorated_data.hpp"

namespace virtual_void::data {
template <typename META>
using unique = erased_unique<decoration_base<META>>;
}  // namespace virtual_void::data
