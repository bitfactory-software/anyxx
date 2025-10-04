#pragma once

#include <virtual_void/runtime/meta_data.hpp>

#include <example_whole_picture/layer_1_core/core.hpp>

namespace whole_picture::core {

struct line {
  architecture::point p1 = {0, 0}, p2 = {0, 0};
  CORE_EXPORT void draw(architecture::mutable_observed_surface const& to) const;
};

}  // namespace whole_picture::core


