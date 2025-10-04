#include <example_whole_picture/layer_1_core/shapes/line/factory.hpp>
#include <example_whole_picture/layer_1_core/shapes/line/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;

using namespace virtual_void;
using namespace virtual_void::data;
using namespace virtual_void::runtime;

VV_RUNTIME_IMPEMENTATION(line)
class_<line>::implements<architecture::shape> __;

shape core::make_line(architecture::point p1, architecture::point p2) {
  return std::make_shared<line>(p1, p2);
}
