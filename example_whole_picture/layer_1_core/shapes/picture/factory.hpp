#pragma once

#include <example_whole_picture/layer_0_architecture/picture.hpp>
#include <example_whole_picture/layer_1_core/core.hpp>

namespace whole_picture::core::shapes {

CORE_EXPORT shape make_picture(architecture::point top_left,
                               architecture::picture& content);

}  // namespace whole_picture::core

