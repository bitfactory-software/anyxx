#pragma once

#include "../virtual_void.h"

namespace virtual_void {

template <typename VIRTUAL_VOID, typename VOID, typename META>
struct observer_trait {
  using void_t = VOID;
  using meta_t = META;
  static constexpr bool is_const = is_const_void<void_t>;

  static void_t value(const auto& ptr) { return ptr.ptr_; }
  static auto meta(const auto& ptr) { return ptr.get_meta(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr.ptr_); }

  template <typename V>
  static auto construct_from(V& v) {
    return VIRTUAL_VOID(static_cast<void_t>(&v), meta_t(std::in_place_type<V>));
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires(is_const)
  {
    return VIRTUAL_VOID(static_cast<void_t>(&v), meta_t(std::in_place_type<V>));
  }

  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V& arg) {
    return construct_from(arg);
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, const V& arg)
    requires VIRTUAL_VOID::is_const
  {
    return construct_from(arg);
  }
};

}  // namespace virtual_void
