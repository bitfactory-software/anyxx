#pragma once

#include <bit_factory/anyxx.hpp>
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

using shape = architecture::shape<anyxx::shared_const>;

}  // namespace whole_picture::core
