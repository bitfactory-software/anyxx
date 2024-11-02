#pragma once

#include "memory"

namespace virtual_void::data {
template <typename ERASED_TO>
using erased_shared_const_ptr = std::shared_ptr<ERASED_TO const>;

template <typename ERASED_TO, typename FROM>
erased_shared_const_ptr<ERASED_TO> erase_shared_const_ptr(const FROM& from) {
  return static_pointer_cast<ERASED_TO>(from);
}

}  // namespace virtual_void::data