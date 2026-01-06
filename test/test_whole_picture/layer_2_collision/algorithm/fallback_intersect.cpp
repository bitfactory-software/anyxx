#include <test/test_whole_picture/layer_2_collision/algorithm/fallback_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>
#include <test/test_whole_picture/layer_1_core/shape_to_picture.hpp>

namespace whole_picture::collision::fallback {

bool intersect(architecture::shape<anyxx::const_observer> const& lhs,
               architecture::shape<anyxx::const_observer> const& rhs) {
  auto picture = core::shape_to_picture(lhs);
  return pictures::intersect_with_picture(picture, rhs);
}

}  // namespace whole_picture::collision::fallback
