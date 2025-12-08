#include <test/test_whole_picture/layer_2_collision/algorithm/fallback_intersect.hpp>
#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>

namespace whole_picture::collision::fallback {

bool intersect(architecture::shape<anyxx::const_observer> const& lhs,
               architecture::shape<anyxx::const_observer> const& rhs) {
  core::shapes::picture lhs_picture{lhs.top_left(), core::surface{lhs.size()}};
  architecture::surface<anyxx::mutable_observer> lhs_surface{lhs_picture};
  lhs.draw(lhs_surface);
  return pictures::intersect(lhs_picture, rhs);
}

}  // namespace whole_picture::collision::fallback
