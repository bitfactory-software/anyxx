#pragma once

#include "erased_unique_ptr.hpp"
#include "decorated_ptr.hpp"

namespace virtual_void::data {
template <typename META>
using unique_ptr = decorated_ptr<void_erased_unique_ptr, META>;
}  // namespace virtual_void::data
