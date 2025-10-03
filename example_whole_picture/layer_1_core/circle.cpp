#include "circle.hpp"

#include <cmath>
#include <ranges>

using namespace whole_picture;
using namespace whole_picture::core;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::runtime;
using namespace virtual_void::data;

VV_RUNTIME_IMPEMENTATION(circle)
class_<circle>::implements<architecture::shape> __;

void circle::draw(architecture::mutable_observed_surface const& surface) const {
  for (int x = -radius; x < radius + 1; ++x)
    for (int y = -radius; y < radius + 1; ++y) {
        auto alpha = std::asin(y /radius);
        auto si = std::sin(alpha);
        auto co = std::cos(alpha);
        auto p = architecture::size{.cx = (int)std::round(std::cos(alpha)) * x,
                                    .cy = y};
        surface.write(center + p, '.');
    }
}

shape core::make_circle(architecture::point center, std::size_t radius) {
  return std::make_shared<circle>(center, radius);
}
