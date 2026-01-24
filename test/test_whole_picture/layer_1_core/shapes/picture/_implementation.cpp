#include <test/test_whole_picture/layer_1_core/shapes/picture/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;
using namespace whole_picture::core::shapes;

using namespace anyxx;

ANY_META_CLASS(shapes::picture)
ANY_MEMBERS_COUNT_IMPL(whole_picture::core::shapes, picture)
ANY_REGISTER_MODEL(shapes::picture, whole_picture::architecture::shape);
//ANY_MODEL(shapes::picture, whole_picture::architecture, surface);
ANY_DISPATCH_FOR(shapes::picture, whole_picture::architecture, shape)

shape shapes::make_picture(architecture::point top_left,
                           core::surface& content) {
  return std::make_shared<picture>(top_left, content);
}
