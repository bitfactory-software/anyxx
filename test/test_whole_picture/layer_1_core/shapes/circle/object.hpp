#pragma once

#include <anyxx/anyxx.hpp>
#include <test/test_whole_picture/layer_1_core/core.hpp>

namespace whole_picture::core::shapes {

struct circle {
  architecture::point center = {0, 0};
  int radius = 1;
};

}  // namespace whole_picture::core

ANY_META_CLASS_FWD(CORE_EXPORT, whole_picture::core::shapes::circle)
ANY_MODEL_FWD(CORE_EXPORT, whole_picture::core::shapes::circle, whole_picture::architecture, shape)
