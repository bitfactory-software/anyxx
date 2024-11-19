#pragma once

#include "erased_shared_const_ptr.hpp"
#include "decorated_ptr.hpp"

namespace virtual_void::data {
template <typename META>
using shared_const_ptr = decorated_ptr<erased_shared_const_ptr<void const>, META>;
}  // namespace virtual_void::data