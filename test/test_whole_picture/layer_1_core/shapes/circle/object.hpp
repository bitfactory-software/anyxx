#pragma once

#include <bit_factory/anyxx.hpp>
#include <test/test_whole_picture/layer_0_architecture/draw/circle.hpp>
#include <test/test_whole_picture/layer_1_core/core.hpp>

namespace whole_picture::core::shapes {

struct circle {
  architecture::point center = {0, 0};
  int radius = 1;
};

}  // namespace whole_picture::core::shapes

ANY_META_CLASS_FWD(CORE_EXPORT, whole_picture::core::shapes::circle)
ANY_DISPATCH_FOR_FWD(CORE_EXPORT, whole_picture::core::shapes::circle,
                     whole_picture::architecture, any_shape)

ANY_MODEL_MAP((whole_picture::core::shapes::circle), whole_picture::architecture::shape) {
  using circle = whole_picture::core::shapes::circle;
  void draw(circle const& self,
            architecture::mutable_observed_surface const& surface) const {
    architecture::draw::circle(surface, self.center, self.radius);
  };
  [[nodiscard]] architecture::size size(circle const& self) const {
    return {.cx = self.radius * 2, .cy = self.radius * 2};
  };
  [[nodiscard]] architecture::point top_left(circle const& self) const {
    return self.center -
           architecture::size{.cx = self.radius, .cy = self.radius};
  };
};
