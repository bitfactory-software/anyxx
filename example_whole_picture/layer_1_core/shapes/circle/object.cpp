#include <example_whole_picture/layer_0_architecture/draw/circle.hpp>
#include <example_whole_picture/layer_1_core/shapes/circle/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;
using namespace whole_picture::architecture;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::runtime;
using namespace virtual_void::data;

void circle::draw(architecture::mutable_observed_surface const& surface) const {

  architecture::draw::circle(surface, center, radius);
}
