#pragma once

//#include <virtual_void/data/decorated_ptr.hpp>
//#include <virtual_void/data/erased_std_ptr_trait.hpp>
//#include <virtual_void/data/erased_unique_ptr.hpp>
#include <virtual_void/data/unique.hpp>

namespace virtual_void::data {
using unique_ptr = unique;
}  // namespace virtual_void::data

//namespace virtual_void {
//template <>
//struct erased_data_trait<data::unique_ptr>
//    : erased_std_ptr_trait<std::unique_ptr, void*> {
//  static constexpr bool is_constructibile_from_const = true;
//  template <typename V>
//  static auto construct_from(std::unique_ptr<V, std::default_delete<V>>&& p) {
//    return data::unique_ptr{data::void_erase_unique_ptr(std::move(p))};
//  }
//  template <typename V>
//  static auto construct_in_place(std::in_place_type_t<V>, V&& arg) {
//    return construct_from(std::forward<V>(arg));
//  }
//};
//}  // namespace virtual_void
//
//namespace virtual_void::data {
//static_assert(has_erased_data_trait<unique_ptr>);
//static_assert(is_erased_data<unique_ptr>);
//}  // namespace virtual_void::data
