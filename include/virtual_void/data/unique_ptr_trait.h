#pragma once

#include "../virtual_void.h"
#include "decorated_ptr_trait.h"
#include "unique_ptr.h"

namespace virtual_void {

template <typename META>
struct unique_ptr_trait : decorated_ptr_trait<void*, META> {
  static void const* value(const auto& ptr) { return ptr.ptr_.get(); }

  template <typename>
  struct unerased_type_impl;
  template <typename T>
  struct unerased_type_impl<std::unique_ptr<T>> {
    using value_type = T;
  };
  template <typename CONSTRUCTED_WITH>
  using unerased_type = unerased_type_impl<CONSTRUCTED_WITH>::value_type;

  template <typename V>
  static auto construct_from(std::unique_ptr<V>&& v) {
    return data::unique_ptr<META>{
        static_pointer_cast<void const>(v),
        META(std::in_place_type<std::decay_t<V>>)};
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V& arg) {
    return construct_from(arg);
  }
};

}  // namespace virtual_void
