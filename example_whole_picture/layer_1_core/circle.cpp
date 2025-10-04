#include "circle.hpp"

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
  auto writeToAllQuadrants = [&](int x1, int x2, int y1, int y2) {
    surface.write({x1 + center.x, -y1 + center.y}, '.');
    surface.write({-x1 + center.x, -y1 + center.y}, '.');
    surface.write({x2 + center.x, y2 + center.y}, '.');
    surface.write({-x2 + center.x, y2 + center.y}, '.');
  };

  int x = radius, y = 0;

  surface.write({x + center.x, y + center.y}, '.');
  if (radius > 0) writeToAllQuadrants(x, y, y, x);

  int P = 1 - radius;
  while (x > y) {
    y++;

    if (P <= 0)
      P = P + 2 * y + 1;
    else
      P = P + 2 * y - 2 * --x + 1;

    if (x < y) break;

    writeToAllQuadrants(x, x, y, y);
    if (x != y) writeToAllQuadrants(y, y, x, x);
  }

  surface.write({center.x, -radius + center.y}, '.');
}

shape core::make_circle(architecture::point center, int radius) {
  return std::make_shared<circle>(center, radius);
}
