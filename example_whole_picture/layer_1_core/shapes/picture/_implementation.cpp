#include <example_whole_picture/layer_0_architecture/draw/picture.hpp>
#include <example_whole_picture/layer_1_core/shapes/picture/factory.hpp>
#include <example_whole_picture/layer_1_core/shapes/picture/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;
using namespace whole_picture::core::shapes;

using namespace virtual_void;
using namespace virtual_void;
using namespace virtual_void;

VV_RUNTIME_INSTANCE(shapes::picture)
VV_V_TABLE_INSTANCE(shapes::picture, whole_picture::architecture, shape);
VV_MEMBERS_COUNT_IMPL(whole_picture::core::shapes, picture)


template <>
struct architecture::shape_v_table_map<shapes::picture>
    : architecture::shape_default_v_table_map<picture> {
  void draw(shapes::picture const* self,
            architecture::mutable_observed_surface const& surface) const {
    architecture::draw::picture(surface, self->top_left, self->content);
  }
};
template <>
struct architecture::surface_v_table_map<shapes::picture>
    : architecture::surface_default_v_table_map<picture> {
  void write(shapes::picture* self, point p, char ch) {
    self->content.write(p, ch);
  }
};

shape shapes::make_picture(architecture::point top_left,
                           architecture::picture& content) {
  return std::make_shared<picture>(top_left, content);
}
