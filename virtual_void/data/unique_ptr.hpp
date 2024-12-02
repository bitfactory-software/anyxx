#pragma once

#include <virtual_void/data/erased_unique_ptr.hpp>
#include <virtual_void/data/decorated_ptr.hpp>

namespace virtual_void::data {
template <typename META>
using unique_ptr = decorated_ptr<void_erased_unique_ptr, META>;
}  // namespace virtual_void::data
