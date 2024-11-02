#pragma once

#include "erased_unique_ptr.h"
#include "decorated_ptr.h"

namespace virtual_void::data {
template <typename META>
using unique_ptr = decorated_ptr<erased_unique_ptr<void>, META>;
}  // namespace virtual_void::data#pragma once
