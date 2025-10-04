#include "circle.hpp"

#include <ranges>

using namespace whole_picture;
using namespace whole_picture::core;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::runtime;
using namespace virtual_void::data;

void circle::draw(architecture::mutable_observed_surface const& surface) const {
  const auto ch = '*';
  auto writeToAllQuadrants = [&](int x1, int x2, int y1, int y2) {
    surface.write({x1 + center.x, -y1 + center.y}, ch);
    surface.write({-x1 + center.x, -y1 + center.y}, ch);
    surface.write({x2 + center.x, y2 + center.y}, ch);
    surface.write({-x2 + center.x, y2 + center.y}, ch);
  };

  int x = radius, y = 0;

  surface.write({x + center.x, y + center.y}, ch);
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

  surface.write({center.x, -radius + center.y}, ch);
}
