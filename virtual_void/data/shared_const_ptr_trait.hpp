#pragma once

#include <virtual_void/virtual_void.hpp>
#include <virtual_void/data/erased_std_ptr_trait.hpp>
#include <virtual_void/data/shared_const_ptr.hpp>

namespace virtual_void {

struct shared_const_ptr_trait
    : erased_std_ptr_trait<std::shared_ptr, void const*> {
  static constexpr bool is_constructibile_from_const = true;
  template <typename V>
  static auto construct_from(std::shared_ptr<V> const& v) {
    return data::shared_const_ptr{
        static_pointer_cast<void const>(v)};
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V& arg) {
    return construct_from(arg);
  }
};

}  // namespace virtual_void
