#pragma once

#include "erased_shared_const_ptr.h"
#include "decorated_ptr.h"

namespace virtual_void::data {
template <typename META>
using shared_const_ptr = decorated_ptr<erased_shared_const_ptr<void>, META>;
}  // namespace virtual_void::data