#pragma once

#include <bit_factory/anyxx.hpp>
#include "../layer_0_architecture/architecture.hpp"

#ifndef CORE_EXPORT
#define CORE_EXPORT
#endif

#ifdef _MSVC_LANG
#ifdef CORE_SHARED
#pragma message("CORE_SHARED -> export")
#undef CORE_EXPORT
#define CORE_EXPORT __declspec(dllexport)
#else
#pragma message ("CORE_SHARED -> import")
#endif
#endif

namespace whole_picture::core {

using shape = architecture::shape<anyxx::shared_const>;

}  // namespace whole_picture::core
