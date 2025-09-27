#pragma once

#include <virtual_void/virtual_void.hpp>

namespace virtual_void {

template <typename VV_VOID>
struct decorated_ptr_trait : virtual_void_default_unerase {
  using void_t = VV_VOID;
  static constexpr bool is_const = is_const_void<void_t>;

  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr.ptr_); }
};

}  // namespace virtual_void
