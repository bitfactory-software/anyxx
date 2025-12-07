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
inline point operator+(point p, size s) {
  return {.x = p.x + s.cx, .y = p.y + s.cy};
};
inline size operator-(size p) { return {.cx = -p.cx, .cy = -p.cy}; }
inline size operator+(size l, size r) {
  return {.cx = l.cx + r.cx, .cy = l.cy + r.cy};
};

ANY(surface, (ANY_METHOD(void, write, (point, char), )))
using mutable_observed_surface = surface<anyxx::mutable_observer>;
using unique_surface = surface<anyxx::unique>;

struct shape_has_dispatch {};
ANY(shape, (ANY_METHOD(void, draw, (mutable_observed_surface), const)))

}  // namespace whole_picture::architecture

ANY_DISPATCH_COUNT_FWD(ARCHITECTURE_EXPORT, whole_picture::architecture, shape)
