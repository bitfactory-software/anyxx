#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../virtual_void.h"

namespace virtual_void {

template <typename VIRTUAL_VOID>
struct observer_trait {
  using void_t = VIRTUAL_VOID::void_t;
  static void_t value(const auto& ptr) { return ptr.ptr_; }
  static auto meta(const auto& ptr) { return ptr.get_meta(); }
  static bool has_value(const auto& ptr) {
    return static_cast<bool>(ptr.ptr_);
  }
  template <typename V>
  static auto construct_from(V& v) {
    return VIRTUAL_VOID(v);
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires VIRTUAL_VOID::is_const
  {
    return VIRTUAL_VOID(v);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, ARG& arg) {
    return VIRTUAL_VOID(arg);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, const ARG& arg)
    requires VIRTUAL_VOID::is_const
  {
    return VIRTUAL_VOID(arg);
  }
};

}  // namespace virtual_void
