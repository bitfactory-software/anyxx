#pragma once

#include <virtual_void/data/decorated_ptr.hpp>
#include <virtual_void/data/erased_std_ptr_trait.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {
using shared_const_ptr = decorated_ptr<std::shared_ptr<void const>>;
}  // namespace virtual_void::data

namespace virtual_void {
template <>
struct erased_data_trait<data::shared_const_ptr>
    : erased_std_ptr_trait<std::shared_ptr, void const*> {
  static constexpr bool is_constructibile_from_const = true;
  template <typename V>
  static auto construct_from(std::shared_ptr<V> const& v) {
    return data::shared_const_ptr{static_pointer_cast<void const>(v)};
  }
  template <typename V>
  static auto construct_in_place(std::in_place_type_t<V>, V& arg) {
    return construct_from(arg);
  }
};
}  // namespace virtual_void

namespace virtual_void::data::has_no_meta {
static_assert(has_erased_data_trait<shared_const_ptr>);
static_assert(is_erased_data<shared_const_ptr>);
}  // namespace virtual_void::data::has_no_meta