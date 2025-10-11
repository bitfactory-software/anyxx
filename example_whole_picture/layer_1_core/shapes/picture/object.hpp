#pragma once

#include <example_whole_picture/layer_0_architecture/picture.hpp>
#include <example_whole_picture/layer_1_core/core.hpp>
#include <virtual_void/open_object/members.hpp>

namespace whole_picture::core::shapes {
struct picture;
}
namespace virtual_void::open_object{
template<> CORE_EXPORT std::size_t& members_count<whole_picture::core::shapes::picture>();/*{
    static std::size_t& get();
};*/
}
namespace whole_picture::core::shapes {

struct picture : virtual_void::open_object::members<picture> {
  picture(architecture::point top_left_, architecture::picture content_)
      : top_left(top_left_), content(content_) {}
  architecture::point top_left;
  architecture::picture content;
};


}  // namespace whole_picture::core::shapes

VV_RUNTIME_FWD(CORE_EXPORT, whole_picture::core::shapes::picture)
VV_V_TABLE_INSTANCE_FWD(CORE_EXPORT, whole_picture::core::shapes::picture,
                        whole_picture::architecture::shape)
VV_V_TABLE_INSTANCE_FWD(CORE_EXPORT, whole_picture::core::shapes::picture,
                        whole_picture::architecture::surface)