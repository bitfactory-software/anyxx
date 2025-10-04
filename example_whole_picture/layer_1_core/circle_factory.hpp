#pragma once

#include "core.hpp"

namespace whole_picture::core {

struct circle;
CORE_EXPORT shape make_circle(architecture::point center, int radius);

}  // namespace whole_picture::core

VV_RUNTIME(CORE_EXPORT, whole_picture::core::circle)
