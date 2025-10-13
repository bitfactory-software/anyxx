#pragma once

#include <example_whole_picture/layer_1_core/core.hpp>
#include <anypp/meta_data.hpp>
#include <anypp/utillities/VIRTUAL_DESTRUCTOR_FOR_DEBUGGING.hpp>

namespace whole_picture::core::shapes {

struct line {
  line(architecture::point p1_, architecture::point p2_) : p1(p1_), p2(p2_) {}
  architecture::point p1 = {0, 0}, p2 = {0, 0};
  VIRTUAL_DESTRUCTOR_FOR_DEBUGGING_DEFAULT(line);
};

}  // namespace whole_picture::core

VV_RUNTIME_FWD(CORE_EXPORT, whole_picture::core::shapes::line)
VV_V_TABLE_INSTANCE_FWD(CORE_EXPORT, whole_picture::core::shapes::line, whole_picture::architecture, shape)