#pragma once

#include <memory>

#include <virtual_void/data/decorated_data.hpp>

namespace virtual_void::data {
using shared_const = std::shared_ptr<decoration_base const>;
}  // namespace virtual_void::data