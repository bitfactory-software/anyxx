#pragma once

#include <virtual_void/data/decorated_ptr.hpp>

namespace virtual_void::data {
using shared_const_ptr = decorated_ptr<std::shared_ptr<void const>>;
}  // namespace virtual_void::data