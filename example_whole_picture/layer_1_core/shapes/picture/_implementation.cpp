#include <example_whole_picture/layer_0_architecture/draw/picture.hpp>
#include <example_whole_picture/layer_1_core/shapes/picture/factory.hpp>
#include <example_whole_picture/layer_1_core/shapes/picture/object.hpp>

using namespace whole_picture;
using namespace whole_picture::core;

using namespace virtual_void;
using namespace virtual_void::data;
using namespace virtual_void::runtime;

VV_RUNTIME_IMPEMENTATION(core::picture)
class_<core::picture>::implements<architecture::shape> __;
class_<core::picture>::implements<architecture::surface> __;

template <>
struct architecture::shape_v_table_map<core::picture>
    : architecture::shape_default_v_table_map<picture> {
  void draw(core::picture const* self,
            architecture::mutable_observed_surface const& surface) const {
    architecture::draw::picture(surface, self->top_left, self->content);
  }
};
template <>
struct architecture::surface_v_table_map<core::picture>
    : architecture::surface_default_v_table_map<picture> {
  void write(core::picture* self, point p, char ch) {
    self->content.write(p, ch);
  }
};

shape core::make_picture(architecture::point top_left,
                         architecture::picture& content) {
  return std::make_shared<picture>(top_left, content);
}
