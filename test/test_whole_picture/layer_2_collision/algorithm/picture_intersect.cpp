#include <test/test_whole_picture/layer_2_collision/algorithm/picture_intersect.hpp>

using namespace whole_picture;

namespace {
struct hitpad_picture_t {
  core::shapes::picture const& picture;
  bool hit = false;
};
}  // namespace

ANY_MODEL_MAP((hitpad_picture_t), architecture::surface) {
  void write(hitpad_picture_t & self, architecture::point p,
             [[maybe_unused]] char ch) {
    if (collision::pictures::intersect_point(
            self.picture, architecture::as_point(p - self.picture.top_left),
            ch))
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
  architecture::surface<anyxx::mutable_observer, anyxx::dyn> hitpad{
      hitpad_picture};
  rhs.draw(hitpad);
  return hitpad_picture.hit;
}
