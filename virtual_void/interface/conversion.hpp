#pragma once

#undef interface

#include <virtual_void/data/copy_convert.hpp>
#include <virtual_void/data/move_convert.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::interface {

template <typename TO_INTERFACE, is_virtual_void VV_FROM>
auto find_v_table(VV_FROM const& vv_from) {
  using v_table_t = typename TO_INTERFACE::v_table_t;
  auto i_table_idx = meta::i_table_index<typename TO_INTERFACE::v_table_t>();
  auto i_table = get_meta(vv_from)->get_i_table();
  auto v_table = i_table->at(i_table_idx);
  return static_cast<TO_INTERFACE::v_table_t*>(v_table);
}

template <typename TO_INTERFACE, is_virtual_void VV_FROM>
TO_INTERFACE attach_interface(VV_FROM const& vv_from) {
  using vv_to_t = typename TO_INTERFACE::virtual_void_t;
  static_assert(is_virtual_void<vv_to_t>);
  auto v_table = find_v_table<TO_INTERFACE>(vv_from);
  return TO_INTERFACE{copy_convert_to<vv_to_t>(vv_from), v_table};
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
TO_INTERFACE attach_interface(const FROM_INTERFACE& from_interface) {
  return attach_interface<TO_INTERFACE>(get_virtual_void(from_interface));
}

template <typename TO_INTERFACE, is_virtual_void VV_FROM>
TO_INTERFACE move_to_interface(VV_FROM&& vv_from) {
  using vv_to_t = typename TO_INTERFACE::virtual_void_t;
  static_assert(is_virtual_void<vv_to_t>);
  auto v_table = find_v_table<TO_INTERFACE>(vv_from);
  return TO_INTERFACE{move_convert_to<vv_to_t>(std::move(vv_from)), v_table};
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
TO_INTERFACE move_to_interface(FROM_INTERFACE&& from_interface) {
  return move_to_interface<TO_INTERFACE>(
      move_virtual_void(std::move(from_interface)));
}

};  // namespace virtual_void::interface