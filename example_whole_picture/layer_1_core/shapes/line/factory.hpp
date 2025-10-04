#pragma once

#include <example_whole_picture/layer_1_core/core.hpp>

namespace whole_picture::core {

struct line;
CORE_EXPORT shape make_line(architecture::point p1, architecture::point p2);

}  // namespace whole_picture::core

VV_RUNTIME(CORE_EXPORT, whole_picture::core::line)
