#pragma once

#include "memory"

namespace virtual_void::data {
template <typename ERASED_TO>
using erased_shared_const_ptr = std::shared_ptr<ERASED_TO const>;
}  // namespace virtual_void::data