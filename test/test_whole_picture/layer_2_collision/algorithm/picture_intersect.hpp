#pragma once

#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>

namespace whole_picture::collision::pictures {

using point = architecture::point;

inline bool intersect(core::shapes::picture const& lhs,
                      core::shapes::picture const& rhs) {
  for (auto ly = 0; ly < lhs.content.get_size().cy; ly++)
    for (auto lx = 0; lx < lhs.content.get_size().cy; lx++)
      if (rhs.content.contains(lhs.top_left - as_size(rhs.top_left) +
                               architecture::size{lx, ly}))
        return true;
  return false;
}

bool intersect(core::shapes::picture const& lhs,
               architecture::shape<anyxx::const_observer> const& rhs);

}  // namespace whole_picture::collision::pictures
