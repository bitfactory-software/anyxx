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

ARCHITECTURE_BASE_EXPORT std::string hello();

}  // namespace whole_picture::core
