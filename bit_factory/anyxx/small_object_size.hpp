#pragma once

#include <bit_factory/anyxx/voidness.hpp>

namespace anyxx {

constexpr std::size_t small_object_size = 4 * sizeof(mutable_void);

}  // namespace anyxx