#pragma once

#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/object.hpp>

namespace whole_picture::collision::lines {

using point = whole_picture::architecture::point;

inline bool on_segment(point p, point q, point r) {
  return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
          q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
}

enum class orientation_type { collinear, clockwise, counter_clockwise };
inline orientation_type orientation(point p, point q, point r) {
  auto use = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
  if (use == 0) return orientation_type::collinear;
  return (use > 0) ? orientation_type::clockwise
                   : orientation_type::counter_clockwise;
}

inline bool intersect(point p1, point q1, point p2, point q2) {
  auto o1 = orientation(p1, q1, p2);
  auto o2 = orientation(p1, q1, q2);
  auto o3 = orientation(p2, q2, p1);
  auto o4 = orientation(p2, q2, q1);
  // general case
  if (o1 != o2 && o3 != o4) return true;
  // special cases
  // p1, q1 and p2 are collinear and p2 lies on segment p1q1
  if (o1 == orientation_type::collinear && on_segment(p1, p2, q1)) return true;
  // p1, q1 and q2 are collinear and q2 lies on segment p1q1
  if (o2 == orientation_type::collinear && on_segment(p1, q2, q1)) return true;
  // p2, q2 and p1 are collinear and p1 lies on segment p2q2
  if (o3 == orientation_type::collinear && on_segment(p2, p1, q2)) return true;
  // p2, q2 and q1 are collinear and q1 lies on segment p2q2
  if (o4 == orientation_type::collinear && on_segment(p2, q1, q2)) return true;
  return false;
}

inline bool intersect(core::shapes::line const& l1, core::shapes::line const& l2) {
  return intersect(l1.p1, l1.p2, l2.p1, l2.p2);
}
}  // namespace whole_picture::collision
