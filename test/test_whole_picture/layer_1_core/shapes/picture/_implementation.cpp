#include <test/test_whole_picture/layer_0_architecture/draw/picture.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;
using namespace whole_picture::core::shapes;

using namespace anyxx;
using namespace anyxx;
using namespace anyxx;

ANY_META_CLASS(shapes::picture)
ANY_MEMBERS_COUNT_IMPL(whole_picture::core::shapes, picture)
ANY_MODEL(shapes::picture, whole_picture::architecture, shape);
ANY_MODEL_MAP((shapes::picture), whole_picture::architecture::shape) {
  void draw(shapes::picture const& self,
            architecture::mutable_observed_surface const& surface) const {
    architecture::draw::picture(surface, self.top_left, self.content);
  };
};
ANY_MODEL(shapes::picture, whole_picture::architecture, surface);
ANY_MODEL_MAP((shapes::picture), whole_picture::architecture::surface) {
  void write(shapes::picture & self, point p, char ch) {
    self.content.write(p, ch);
  };
};

shape shapes::make_picture(architecture::point top_left,
                           core::surface& content) {
  return std::make_shared<picture>(top_left, content);
}
