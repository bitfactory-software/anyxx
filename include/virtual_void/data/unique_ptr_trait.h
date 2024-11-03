#pragma once

#include "../virtual_void.h"
#include "erased_std_ptr_trait.h"
#include "unique_ptr.h"

namespace virtual_void {

template <typename META>
struct unique_ptr_trait : erased_std_ptr_trait<std::unique_ptr, void*, META> {
  template <typename V>
  static auto construct_from(std::unique_ptr<V>&& p) {
    return data::unique_ptr<META>{data::void_erase_unique_ptr(std::move(p)),
                                  META(std::in_place_type<std::decay_t<V>>)};
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V&& arg) {
    return construct_from(std::forward<V>(arg));
  }
};

}  // namespace virtual_void
