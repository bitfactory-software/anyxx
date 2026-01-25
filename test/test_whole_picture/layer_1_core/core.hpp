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
#endif
#endif

namespace whole_picture::core {

using shape = architecture::any_shape<anyxx::shared>;

}  // namespace whole_picture::core
