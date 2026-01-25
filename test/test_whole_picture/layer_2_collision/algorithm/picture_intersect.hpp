#pragma once

#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>

namespace whole_picture::collision::pictures {

using point = architecture::point;

inline bool intersect_point(core::shapes::picture const& lhs,
                            architecture::point p_in_content, char rhs_ch) {
  if (rhs_ch == ' ') return false;
  if (!lhs.content.contains(p_in_content)) return false;
  if (lhs.content.at(p_in_content) == ' ') return false;
  return true;
}

inline bool intersect(core::shapes::picture const& lhs,
                      core::shapes::picture const& rhs) {
  for (auto ry = 0; ry < rhs.content.get_size().cy; ry++)
    for (auto rx = 0; rx < rhs.content.get_size().cy; rx++)
      if (auto rhs_ch = rhs.content.at({rx, ry}); intersect_point(
              lhs,
              architecture::as_point(architecture::size{.cx = rx, .cy = ry} +
                                     (rhs.top_left - lhs.top_left)),
              rhs_ch))
        return true;
  return false;
}

bool intersect(
    core::shapes::picture const& lhs,
    architecture::any_shape<anyxx::cref> const& rhs);
}  // namespace whole_picture::collision::pictures
