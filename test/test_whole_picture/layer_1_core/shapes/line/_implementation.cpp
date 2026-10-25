#include <test/test_whole_picture/layer_0_architecture/draw/line.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/factory.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/line/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;
using namespace whole_picture::core::shapes;

using namespace anyxx;

META_CLASS(line)
namespace {
enable_dynamic<shapes::line, whole_picture::architecture::shape> __;
}
ANY_DISPATCH_FOR(line, whole_picture::architecture, shape)


shape shapes::make_line(architecture::point p1, architecture::point p2) {
  return std::make_shared<line>(p1, p2);
}
