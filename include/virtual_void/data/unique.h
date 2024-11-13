#pragma once

#include "erased_unique.h"
#include "decorated_data.h"

namespace virtual_void::data {
template <typename META>
using unique = erased_unique<decoration_base<META>>;
}  // namespace virtual_void::data
