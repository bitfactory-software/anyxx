#pragma once

#include <virtual_void/runtime/meta_data.hpp>

#include "core.hpp"

namespace whole_picture::core {

struct circle {
  architecture::point center = {0, 0};
  int radius = 1;
  CORE_EXPORT void draw(architecture::mutable_observed_surface const& to) const;
};

}  // namespace whole_picture::core


