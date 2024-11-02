#pragma once

#include "../virtual_void.h"

namespace virtual_void {

template <typename VOID, typename META>
struct decorated_ptr_trait{
  using void_t = VOID;
  using meta_t = META;
  static constexpr bool is_const = is_const_void<void_t>;

  static auto meta(const auto& ptr) { return ptr.get_meta(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr.ptr_); }
};

}  // namespace virtual_void
