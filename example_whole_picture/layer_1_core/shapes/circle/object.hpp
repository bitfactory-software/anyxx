#pragma once

#include <virtual_void/runtime/meta_data.hpp>

#include <example_whole_picture/layer_1_core/core.hpp>

namespace whole_picture::core::shapes {

struct circle {
  architecture::point center = {0, 0};
  int radius = 1;
};

}  // namespace whole_picture::core

VV_RUNTIME_FWD(CORE_EXPORT, whole_picture::core::shapes::circle)
VV_V_TABLE_INSTANCE_FWD(CORE_EXPORT, whole_picture::core::shapes::circle, whole_picture::architecture, shape)
