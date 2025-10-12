#pragma once

#include <string>
#include <virtual_void/observer.hpp>
#include <virtual_void/shared_const.hpp>
#include <virtual_void/unique.hpp>
#include <virtual_void/base.hpp>
#include <virtual_void/declare_macro.hpp>
#include <virtual_void/virtual_void.hpp>

using namespace virtual_void;
using namespace virtual_void;

#ifndef ARCHITECTURE_EXPORT
#define ARCHITECTURE_EXPORT
#endif

#ifdef ARCHITECTURE_SHARED
#ifdef _MSVC_LANG
#undef ARCHITECTURE_EXPORT
#define ARCHITECTURE_EXPORT __declspec(dllexport)
#pragma message("architecturue -> dll")
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

VV_ANY(surface, (VV_METHOD(void, write, point, char)))
using mutable_observed_surface = surface<virtual_void::mutable_observer>;
using unique_surface = surface<virtual_void::unique>;

VV_ANY(shape, (VV_CONST_METHOD(void, draw, mutable_observed_surface)))

ARCHITECTURE_EXPORT std::string hello();

}  // namespace whole_picture::architecture
