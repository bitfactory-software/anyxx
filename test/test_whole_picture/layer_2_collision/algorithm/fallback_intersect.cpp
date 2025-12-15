#include <test/test_whole_picture/layer_2_collision/algorithm/fallback_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>
#include <test/test_whole_picture/layer_1_core/shape_to_picture.hpp>

namespace whole_picture::collision::fallback {

bool intersect(architecture::shape<anyxx::const_observer> const& lhs,
               architecture::shape<anyxx::const_observer> const& rhs) {
  return pictures::intersect(core::shape_to_picture(lhs), rhs);
}

}  // namespace whole_picture::collision::fallback
