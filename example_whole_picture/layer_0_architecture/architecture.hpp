#pragma once

#include <string>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>
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

VV_INTERFACE(surface, (VV_METHOD(void, write, point, char)))
using mutable_observed_surface = surface<virtual_void::data::mutable_observer>;
using unique_surface = surface<virtual_void::data::unique>;

VV_INTERFACE(shape, (VV_CONST_METHOD(void, draw, mutable_observed_surface)))

ARCHITECTURE_EXPORT std::string hello();

}  // namespace whole_picture::architecture
