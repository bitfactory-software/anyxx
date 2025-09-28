#pragma once

#include <virtual_void/virtual_void.hpp>

namespace virtual_void {

template <typename ERASED_DATA, typename VV_VOID>
struct observer_trait : virtual_void_default_unerase {
  using void_t = VV_VOID;
  static constexpr bool is_const = is_const_void<void_t>;
  static constexpr bool is_constructibile_from_const = is_const;

  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }
  static VV_VOID value(const auto& ptr) { return ptr; }

  template <typename V>
  static auto construct_from(V& v) {
    static_assert(!std::is_const_v<std::remove_reference_t<V>>);
    return ERASED_DATA(static_cast<VV_VOID>(&v));
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires(is_const)
  {
    return ERASED_DATA(static_cast<VV_VOID>(&v));
  }

  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V& arg) {
    return construct_from(arg);
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, const V& arg)
    requires is_const
  {
    return construct_from(arg);
  }
};

}  // namespace virtual_void
