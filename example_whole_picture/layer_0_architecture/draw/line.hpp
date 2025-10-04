#include <cmath>
#include <example_whole_picture/layer_0_architecture/architecture.hpp>
#include <ranges>

namespace whole_picture::architecture::draw{

void draw(auto const surface, point p1, point p2){
  if (p1 == p2) return;

  const auto ch = '*';

  p1 = point{std::min(p1.x, p2.x), p1.y};
  p2 = point{std::max(p1.x, p2.x), p2.y};
#
  auto dx = p2.x - p1.x;
  if (!dx) {
    for (auto y : std::views::iota(p1.y, p2.y)) surface.write({p1.x, y}, ch);
    return;
  }
  auto dy = p2.y - p1.y;
  if (!dx) {
    for (auto x : std::views::iota(p1.x, p2.x)) surface.write({x, p1.y}, ch);
    return;
  }

  auto m = dy * 1.0 / dx * 1.0;
  auto last_y = p1.y;
  for (int x = p1.x; x < p2.x; ++x) {
    auto y = static_cast<int>(std::round(p1.x + x * m) + p1.y);
    while (last_y - 1 < y - 1) {
      surface.write({x, ++last_y}, ch);
    }
    surface.write({x, y}, ch);
  }
}
}


