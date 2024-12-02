#pragma once

#include "../virtual_void.hpp"
#include "erased_std_ptr_trait.hpp"
#include "shared_const_ptr.hpp"

namespace virtual_void {

template <typename META>
struct shared_const_ptr_trait
    : erased_std_ptr_trait<std::shared_ptr, void const*, META> {
  template <typename V>
  static auto construct_from(std::shared_ptr<V> const& v) {
    return data::shared_const_ptr<META>{
        static_pointer_cast<void const>(v),
        META(std::in_place_type<std::decay_t<V>>)};
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V& arg) {
    return construct_from(arg);
  }
};

}  // namespace virtual_void
