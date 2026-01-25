#pragma once

#include <bit_factory/anyxx.hpp>
#include <test/test_whole_picture/layer_0_architecture/architecture.hpp>
#include <test/test_whole_picture/layer_1_core/core.hpp>
#include <test/test_whole_picture/layer_1_core/shapes/picture/object.hpp>

namespace whole_picture::core {

CORE_EXPORT shapes::picture shape_to_picture(
    architecture::any_shape<anyxx::cref> const& shape);

}  // namespace whole_picture::core
