#pragma once

#include "../virtual_void.h"
#include "decorated_ptr_trait.h"
#include "shared_const_ptr.h"

namespace virtual_void {

template <typename META>
struct shared_const_ptr_trait : decorated_ptr_trait<void const*, META> {

  static void const* value(const auto& ptr) { return ptr.ptr_.get(); }

  template <typename V>
  static auto construct_from(std::shared_ptr<V> const& v) {
    return data::shared_const_ptr<META>{static_pointer_cast<void const>(v),
                                  META(std::in_place_type<std::decay_t<V>>)};
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V& arg) {
    return construct_from(arg);
  }
};

}  // namespace virtual_void
