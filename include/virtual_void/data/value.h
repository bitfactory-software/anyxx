#pragma once

#include "erased_value.h"
#include "decorated_data.h"

namespace virtual_void::data {
template <typename META>
using value = erased_value<decoration_base<META>>;
}  // namespace virtual_void::data
