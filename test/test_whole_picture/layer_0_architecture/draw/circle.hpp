#pragma once

#include <cmath>
#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <ranges>

namespace whole_picture::architecture::draw{

void circle(auto surface, point center, int radius){
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

}
