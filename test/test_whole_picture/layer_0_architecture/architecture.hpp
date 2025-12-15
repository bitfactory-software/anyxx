#pragma once

#include <bit_factory/anyxx.hpp>

#ifndef ARCHITECTURE_EXPORT
#define ARCHITECTURE_EXPORT
#endif

#ifdef ARCHITECTURE_SHARED
#ifdef _MSVC_LANG
#undef ARCHITECTURE_EXPORT
#define ARCHITECTURE_EXPORT __declspec(dllexport)
#endif
#endif

namespace whole_picture::architecture {

struct point {
  int x = 0, y = 0;
  auto operator<=>(point const&) const = default;
};
struct size {
  int cx = 0, cy = 0;
  auto operator<=>(size const&) const = default;
};

inline point operator-(point p) { return {.x = -p.x, .y = -p.y}; }
inline point operator-(point p, size s) {
  return {.x = p.x - s.cx, .y = p.y - s.cy};
}
inline point operator+(point p, size s) {
  return {.x = p.x + s.cx, .y = p.y + s.cy};
};
inline point operator+(size s, point p) { return p + s; };
inline size operator-(point lhs, point rhs) {
  return {.cx = lhs.x - rhs.x, .cy = lhs.y - rhs.y};
}
inline size operator+(point lhs, point rhs) {
  return {.cx = lhs.x + rhs.x, .cy = lhs.y + rhs.y};
}

inline size operator-(size p) { return {.cx = -p.cx, .cy = -p.cy}; }
inline size operator+(size l, size r) {
  return {.cx = l.cx + r.cx, .cy = l.cy + r.cy};
};
inline size as_size(point p) { return {.cx = p.x, .cy = p.y}; }
inline point as_point(size s) { return {.x = s.cx, .y = s.cy}; }

inline size abs(size p) { return {.cx = std::abs(p.cx), .cy = std::abs(p.cy)}; }
inline point min(point lhs, point rhs) {
  return {.x = std::min(lhs.x, rhs.x), .y = std::min(lhs.y, rhs.y)};
}
inline point max(point lhs, point rhs) {
  return {.x = std::max(lhs.x, rhs.x), .y = std::max(lhs.y, rhs.y)};
}

ANY(surface, (ANY_METHOD(void, write, (point, char), )))
using mutable_observed_surface = surface<anyxx::mutable_observer, anyxx::dyn>;
using unique_surface = surface<anyxx::unique, anyxx::dyn>;

struct shape_has_open_dispatch {};
ANY(shape, (ANY_METHOD(void, draw, (mutable_observed_surface), const),
            ANY_METHOD(architecture::point, top_left, (), const),
            ANY_METHOD(architecture::size, size, (), const)))

}  // namespace whole_picture::architecture

ANY_DISPATCH_COUNT_FWD(ARCHITECTURE_EXPORT, whole_picture::architecture, shape)
ANY_DISPATCH_FOR_FWD(ARCHITECTURE_EXPORT,
                     whole_picture::architecture::shape<anyxx::const_observer>,
                     whole_picture::architecture, shape)
