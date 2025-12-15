#pragma once

#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_2_collision/collision.hpp>

namespace whole_picture::collision::fallback {

COLLISION_EXPORT bool intersect(
    architecture::shape<anyxx::const_observer> const& lhs,
    architecture::shape<anyxx::const_observer> const& rhs);

}  // namespace whole_picture::collision::fallback
