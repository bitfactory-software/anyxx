#pragma once

#include <example_whole_picture/layer_1_core/core.hpp>
#include <anyxx/anyxx.hpp>

namespace whole_picture::core::shapes {

struct line {
  line(architecture::point p1_, architecture::point p2_) : p1(p1_), p2(p2_) {}
  architecture::point p1 = {0, 0}, p2 = {0, 0};
};

}  // namespace whole_picture::core

ANY_META_CLASS_FWD(CORE_EXPORT, whole_picture::core::shapes::line)
ANY_MODEL_FWD(CORE_EXPORT, whole_picture::core::shapes::line, whole_picture::architecture, shape)