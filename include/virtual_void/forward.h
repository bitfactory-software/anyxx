#pragma once

#include <typeindex>

namespace virtual_void {
using type_info_ptr = std::type_info const*;

template <typename>
struct self_pointer;
template <>
struct self_pointer<void*> {
  template <typename CLASS>
  using type = CLASS*;
};
template <>
struct self_pointer<const void*> {
  template <typename CLASS>
  using type = const CLASS*;
};

class error;

}  // namespace virtual_void

#ifdef _DEBUG
#define VIRTUAL_DESTRUCTOR_FOR_DEBUGGING virtual
#else
#define VIRTUAL_DESTRUCTOR_FOR_DEBUGGING
#endif  // DEBUG
