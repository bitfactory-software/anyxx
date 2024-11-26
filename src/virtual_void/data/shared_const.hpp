#pragma once

#include "erased_shared_const.hpp"
#include "decorated_data.hpp"

namespace virtual_void::data {
template <typename META>
using shared_const = erased_shared_const<decoration_base<META>>;
}  // namespace virtual_void::data