#include <bit_factory/anyxx.hpp>
#include <test/test_whole_picture/layer_0_architecture/draw/circle.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/circle/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;
using namespace whole_picture::core::shapes;

using namespace anyxx;

ANY_META_CLASS(circle)
ANY_MODEL(circle, whole_picture::architecture, shape);
ANY_DISPATCH_FOR(circle, whole_picture::architecture, shape)

shape shapes::make_circle(architecture::point center, int radius) {
  return std::make_shared<circle>(center, radius);
}
