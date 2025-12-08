#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>

using namespace whole_picture;

namespace {
struct hitpad_picture_t {
  core::shapes::picture const& picture;
  bool hit = false;
};
}

ANY_META_CLASS(hitpad_picture_t)
ANY_MODEL(hitpad_picture_t, architecture, surface);
ANY_MODEL_MAP((hitpad_picture_t), architecture::surface) {
  void write(hitpad_picture_t & self, architecture::point p,
             [[maybe_unused]] char ch) {
    if (self.picture.content.contains(
            architecture::as_point(p + self.picture.top_left)))
      self.hit = true;
  };
  auto size(hitpad_picture_t const& self) const {
    self.picture.content.get_size();
  };
};

bool collision::pictures::intersect(
    core::shapes::picture const& lhs,
    architecture::shape<anyxx::const_observer> const& rhs) {
  hitpad_picture_t hitpad_picture{.picture = lhs, .hit = false};
  architecture::surface<anyxx::mutable_observer> hitpad{hitpad_picture};
  rhs.draw(hitpad);
  return hitpad_picture.hit;
}


