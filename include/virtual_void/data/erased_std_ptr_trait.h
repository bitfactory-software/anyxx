#pragma once

#include "../virtual_void.h"
#include "decorated_ptr_trait.h"
#include "shared_const_ptr.h"

namespace virtual_void {

template <template< typename > typename STD_PTR, typename VOID, typename META>
struct erased_std_ptr_trait : decorated_ptr_trait<VOID, META> {
  static VOID value(const auto& ptr) { return ptr.ptr_.get(); }

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
