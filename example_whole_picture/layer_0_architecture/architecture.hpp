#pragma once 

#include <string>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/virtual_void.hpp>

using namespace virtual_void;
using namespace virtual_void::interface;

#ifndef ARCHITECTURE_BASE_EXPORT
#define ARCHITECTURE_BASE_EXPORT
#endif

#ifdef ARCHITECTURE_SHARED
#ifdef _MSVC_LANG
#undef ARCHITECTURE_BASE_EXPORT
#define ARCHITECTURE_BASE_EXPORT __declspec(dllexport)
#pragma message("architecturue -> dll")
#endif
#endif

namespace whole_picture::architecture {

struct point {
  int x = 0, y = 0;
};

VV_INTERFACE(surface, (VV_METHOD(void, write, point, char), VV_CONST_METHOD(void, flush)))
using mutable_observed_surface = surface<virtual_void::data::mutable_observer>;
using unique_surface = surface<virtual_void::data::unique>;
ARCHITECTURE_BASE_EXPORT unique_surface screen();

VV_INTERFACE(drawable, (VV_CONST_METHOD(void, draw, mutable_observed_surface)))

ARCHITECTURE_BASE_EXPORT std::string hello();

}  // namespace whole_picture::architecture
