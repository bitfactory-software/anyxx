#include <test/test_whole_picture/layer_1_core/shape_to_picture.hpp>

using namespace whole_picture;

namespace {
struct shape_surface_adapter {
  core::shapes::picture& picture;
  void write(architecture::point p, char ch) {
    picture.content.write(architecture::as_point(p - picture.top_left), ch);
  }
};
}  // namespace

core::shapes::picture core::shape_to_picture(
    architecture::shape<anyxx::const_observer> const& shape) {
  core::shapes::picture picture{shape.top_left(), core::surface{shape.size()}};
  shape_surface_adapter adapter{picture};
  architecture::surface<anyxx::mutable_observer, anyxx::dyn> surface{adapter};
  shape.draw(surface);
  return picture;
}
