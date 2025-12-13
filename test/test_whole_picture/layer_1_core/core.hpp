#pragma once

#include <bit_factory/anyxx.hpp>
#include "../layer_0_architecture/architecture.hpp"

#ifdef _MSVC_LANG
#define CORE_EXPORT __declspec(dllexport)
#else
#define CORE_EXPORT __attribute__((__visibility__("default")))
#endif

namespace whole_picture::core {

using shape = architecture::shape<anyxx::shared_const>;

}  // namespace whole_picture::core
