#pragma once

#include <virtual_void/data/erased_unique_ptr.hpp>
#include <virtual_void/data/decorated_ptr.hpp>

namespace virtual_void::data {
using unique_ptr = decorated_ptr<void_erased_unique_ptr>;
}  // namespace virtual_void::data
