#pragma once

#include <bit_factory/anyxx.hpp>
#include "../layer_0_architecture/architecture.hpp"

#ifndef COLLISIONS_EXPORT
#define CORE_EXPORT
#endif

#ifdef COLLISIONS_SHARED
#ifdef _MSVC_LANG
#undef COLLISIONS_EXPORT
#define COLLISIONS_EXPORT __declspec(dllexport)
#endif
#endif

namespace whole_picture::collision {

using shape = architecture::shape<anyxx::shared_const>;

using happened_dispatch_t = anyxx::dispatch<bool(
    anyxx::virtual_<shape>, anyxx::virtual_<shape>)>;

extern COLLISIONS_EXPORT happened_dispatch_t& happened();


}  // namespace whole_picture::core
