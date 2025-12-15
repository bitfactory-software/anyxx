#pragma once

#include <test/test_whole_picture/layer_1_core/core.hpp>
#include <test/test_whole_picture/layer_1_core/surface/object.hpp>

namespace whole_picture::core::shapes {

CORE_EXPORT shape make_picture(architecture::point top_left, surface& content);

}  // namespace whole_picture::core::shapes
