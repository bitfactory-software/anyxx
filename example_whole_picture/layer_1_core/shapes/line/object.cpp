#include <cmath>
#include <example_whole_picture/layer_1_core/shapes/line/object.hpp>
#include <ranges>

using namespace whole_picture;
using namespace whole_picture::architecture;
using namespace whole_picture::core;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::runtime;
using namespace virtual_void::data;

namespace {
template <typename T>
int sign(T val) {
  return (T(0) < val) - (val < T(0));
}
}  // namespace

void line::draw(architecture::mutable_observed_surface const& surface) const {
  if (p1 == p2) return;

  const auto ch = '*';

  auto p1 = point{std::min(this->p1.x, this->p2.x), this->p1.y};
  auto p2 = point{std::max(this->p1.x, this->p2.x), this->p2.y};
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
  auto s = sign(m);
  auto last_y = p1.y;
  for (int x = p1.x; x < p2.x; ++x) {
    auto y = static_cast<int>(std::round(p1.x + x * m) + p1.y);
    while (last_y - 1 < y - 1) {
      surface.write({x, ++last_y}, ch);
    }
    surface.write({x, y}, ch);
  }
}
