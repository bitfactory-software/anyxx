#pragma once

#include <virtual_void/virtual_void.hpp>
#include <virtual_void/data/decorated_ptr_trait.hpp>

namespace virtual_void {

template <template< typename > typename STD_PTR, typename VV_VOID>
struct erased_std_ptr_trait : decorated_ptr_trait<VV_VOID> {
  static VV_VOID value(const auto& ptr) { return ptr.ptr_.get(); }

  template <typename>
  struct unerased_type_impl;
  template <typename T>
  struct unerased_type_impl<STD_PTR<T>> {
    using value_type = T;
  };
  template <typename CONSTRUCTED_WITH>
  using unerased_type = unerased_type_impl<CONSTRUCTED_WITH>::value_type;
};

}  // namespace virtual_void
