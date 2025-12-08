#pragma once

#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/object.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>
#include <test/test_whole_picture/layer_2_collisions/collision.hpp>

namespace whole_picture::collision {

namespace core_shapes = whole_picture::core::shapes;

COLLISIONS_EXPORT bool happens(core_shapes::circle const& lhs,
                               core_shapes::circle const& rhs);
COLLISIONS_EXPORT bool happens(core_shapes::circle const& lhs,
                               core_shapes::line const& rhs);
COLLISIONS_EXPORT bool happens(core_shapes::circle const& lhs,
                               core_shapes::picture const& rhs);
COLLISIONS_EXPORT bool happens(core_shapes::line const& lhs,
                               core_shapes::line const& rhs);
COLLISIONS_EXPORT bool happens(core_shapes::line const& lhs,
                               core_shapes::picture const& rhs);
COLLISIONS_EXPORT bool happens(core_shapes::picture const& lhs,
                               core_shapes::picture const& rhs);

}  // namespace whole_picture::collision
