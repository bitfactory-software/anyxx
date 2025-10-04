#pragma once

#include <virtual_void/runtime/meta_data.hpp>

#include <example_whole_picture/layer_1_core/core.hpp>

namespace whole_picture::core {

struct circle {
  architecture::point center = {0, 0};
  int radius = 1;
};

}  // namespace whole_picture::core


