#pragma once

#include <bit_factory/anyxx.hpp>

#include "../layer_0_architecture/architecture.hpp"

#ifndef COLLISION_EXPORT
#define COLLISION_EXPORT
#endif

#ifdef COLLISION_SHARED
#ifdef _MSVC_LANG
#undef COLLISION_EXPORT
#define COLLISION_EXPORT __declspec(dllexport)
#endif
#endif

namespace whole_picture::collision {

using happened_dispatch_t = anyxx::dispatch<bool(
    anyxx::virtual_<architecture::shape<anyxx::const_observer>>,
    anyxx::virtual_<architecture::shape<anyxx::const_observer>>)>;

extern COLLISION_EXPORT happened_dispatch_t& happened();

}  // namespace whole_picture::collision
