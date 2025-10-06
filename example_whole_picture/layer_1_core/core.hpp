#pragma once

#include <virtual_void/data/shared_const.hpp>

#include "../layer_0_architecture/architecture.hpp"

#ifndef CORE_EXPORT
#define CORE_EXPORT
#endif

#ifdef CORE_SHARED
#ifdef _MSVC_LANG
#undef CORE_EXPORT
#define CORE_EXPORT __declspec(dllexport)
#pragma message("core -> dll")
#endif
#endif

namespace whole_picture::core {

using shape = architecture::shape<virtual_void::data::shared_const>;

CORE_EXPORT std::string hello();

}  // namespace whole_picture::core
