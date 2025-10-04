#include "circle_factory.hpp"
#include "circle.hpp"

using namespace whole_picture;
using namespace whole_picture::core;

using namespace virtual_void;
using namespace virtual_void::data;
using namespace virtual_void::runtime;

VV_RUNTIME_IMPEMENTATION(circle)
class_<circle>::implements<architecture::shape> __;

shape core::make_circle(architecture::point center, int radius) {
  return std::make_shared<circle>(center, radius);
}
