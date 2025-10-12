#include <example_whole_picture/layer_0_architecture/draw/line.hpp>
#include <example_whole_picture/layer_1_core/shapes/line/factory.hpp>
#include <example_whole_picture/layer_1_core/shapes/line/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;
using namespace whole_picture::core::shapes;

using namespace virtual_void;
using namespace virtual_void;
using namespace virtual_void;

VV_RUNTIME_INSTANCE(line)
VV_V_TABLE_INSTANCE(shapes::line, whole_picture::architecture, shape);

template <>
struct architecture::shape_v_table_map<line>
    : architecture::shape_default_v_table_map<line> {
  void draw(line const* self,
            architecture::mutable_observed_surface const& surface) const {
    architecture::draw::line(surface, self->p1, self->p2);
  }
};

shape shapes::make_line(architecture::point p1, architecture::point p2) {
  return std::make_shared<line>(p1, p2);
}
