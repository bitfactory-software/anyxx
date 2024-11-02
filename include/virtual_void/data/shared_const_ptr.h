#pragma once

#include <memory>

#include "decorated_ptr.h"

namespace virtual_void::data {

template <typename META>
using shared_const_ptr = decorated_ptr<std::shared_ptr<void const>, META>;

}  // namespace virtual_void::data