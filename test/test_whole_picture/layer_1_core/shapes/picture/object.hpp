#pragma once

#include <test/test_whole_picture/layer_0_architecture/picture.hpp>
#include <test/test_whole_picture/layer_1_core/core.hpp>
#include <anyxx/anyxx.hpp>

ANY_MEMBERS_COUNT_FWD(CORE_EXPORT, whole_picture::core::shapes, picture)

namespace whole_picture::core::shapes {

struct picture : anyxx::members<picture> {
  picture(architecture::point top_left_, architecture::picture content_)
      : top_left(top_left_), content(content_) {}
  architecture::point top_left;
  architecture::picture content;
};


}  // namespace whole_picture::core::shapes

ANY_META_CLASS_FWD(CORE_EXPORT, whole_picture::core::shapes::picture)
ANY_MODEL_FWD(CORE_EXPORT, whole_picture::core::shapes::picture,
                        whole_picture::architecture, shape)
ANY_MODEL_FWD(CORE_EXPORT, whole_picture::core::shapes::picture,
                        whole_picture::architecture, surface)