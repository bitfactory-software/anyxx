#pragma once

#include "decorated_ptr_trait.hpp"

namespace virtual_void {

template <typename VIRTUAL_VOID, typename VOID, typename META>
struct observer_trait : decorated_ptr_trait<VOID, META> {
  using base_trait = decorated_ptr_trait<VOID, META>;

  static VOID value(const auto& ptr) { return ptr.ptr_; }

  template <typename V>
  static auto construct_from(V& v) {
    return VIRTUAL_VOID(static_cast<VOID>(&v), META(std::in_place_type<V>));
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires(base_trait::is_const)
  {
    return VIRTUAL_VOID(static_cast<VOID>(&v), META(std::in_place_type<V>));
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
