#pragma once

#include <virtual_void/data/decorated_ptr_trait.hpp>

namespace virtual_void {

template <typename VIRTUAL_VOID, typename VV_VOID, typename META>
struct observer_trait : decorated_ptr_trait<VV_VOID, META> {
  using base_trait = decorated_ptr_trait<VV_VOID, META>;
  static constexpr bool is_constructibile_from_const = base_trait::is_const;

  static VV_VOID value(const auto& ptr) { return ptr.ptr_; }

  template <typename V>
  static auto construct_from(V& v) {
    static_assert(!std::is_const_v<std::remove_reference_t<V>>);
    return VIRTUAL_VOID(static_cast<VV_VOID>(&v), META(std::in_place_type<V>));
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires(base_trait::is_const)
  {
    return VIRTUAL_VOID(static_cast<VV_VOID>(&v), META(std::in_place_type<V>));
  }

  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V& arg) {
    return construct_from(arg);
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, const V& arg)
    requires base_trait::is_const
  {
    return construct_from(arg);
  }
};

}  // namespace virtual_void
