#pragma once

#include <virtual_void/virtual_void.hpp>
#include <virtual_void/data/erased_std_ptr_trait.hpp>
#include <virtual_void/data/unique_ptr.hpp>

namespace virtual_void {

struct unique_ptr_trait : erased_std_ptr_trait<std::unique_ptr, void*> {
  static constexpr bool is_constructibile_from_const = true;
  template <typename V>
  static auto construct_from(std::unique_ptr<V>&& p) {
    return data::unique_ptr{data::void_erase_unique_ptr(std::move(p))};
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V&& arg) {
    return construct_from(std::forward<V>(arg));
  }
};

}  // namespace virtual_void
