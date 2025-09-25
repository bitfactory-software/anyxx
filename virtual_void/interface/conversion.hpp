#pragma once

#undef interface

#include <ranges>
#include <virtual_void/data/copy_convert.hpp>
#include <virtual_void/data/move_convert.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/meta/class.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::interface {

template <typename TO_INTERFACE>
auto find_v_table(meta::type_info* type_info) -> TO_INTERFACE::v_table_t* {
  using v_table_t = typename TO_INTERFACE::v_table_t;
  auto v_table = type_info->get_v_table(typeid(TO_INTERFACE::v_table_t));
  return static_cast<TO_INTERFACE::v_table_t*>(v_table);
}

template <typename TO_INTERFACE>
auto find_v_table(meta::base_v_table* from) -> TO_INTERFACE::v_table_t* {
  using v_table_t = typename TO_INTERFACE::v_table_t;
  return find_v_table<TO_INTERFACE>(from->type_info);
}

template <typename TO_INTERFACE, is_virtual_void VV_FROM>
TO_INTERFACE attach_interface(VV_FROM const& vv_from,
                              meta::type_info* type_info) {
  using vv_to_t = typename TO_INTERFACE::virtual_void_t;
  static_assert(is_virtual_void<vv_to_t>);
  auto v_table = find_v_table<TO_INTERFACE>(type_info);
  return TO_INTERFACE{copy_convert_to<vv_to_t>(vv_from), v_table};
}

template <typename TO_INTERFACE, is_virtual_void VV_FROM>
TO_INTERFACE attach_interface(VV_FROM const& vv_from) {
  using vv_to_t = typename TO_INTERFACE::virtual_void_t;
  static_assert(is_virtual_void<vv_to_t>);
  return attach_interface<TO_INTERFACE>(vv_from,
                                        get_meta(vv_from)->type_info());
}

template <typename TO_INTERFACE, is_virtual_void VV_FROM>
TO_INTERFACE attach_interface(VV_FROM const& vv_from,
                              meta::base_v_table* from) {
  return attach_interface<TO_INTERFACE>(vv_from, from->type_info);
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
TO_INTERFACE attach_interface(const FROM_INTERFACE& from_interface) {
  return attach_interface<TO_INTERFACE>(get_virtual_void(from_interface),
                                        get_v_table(from_interface));
}

template <typename TO_INTERFACE, is_virtual_void VV_FROM>
TO_INTERFACE move_to_interface(VV_FROM&& vv_from) {
  using vv_to_t = typename TO_INTERFACE::virtual_void_t;
  static_assert(is_virtual_void<vv_to_t>);
  auto v_table = find_v_table<TO_INTERFACE>(get_meta(vv_from)->type_info());
  return TO_INTERFACE{move_convert_to<vv_to_t>(std::move(vv_from)), v_table};
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
TO_INTERFACE move_to_interface(FROM_INTERFACE&& from_interface) {
  return move_to_interface<TO_INTERFACE>(
      move_virtual_void(std::move(from_interface)));
}

};  // namespace virtual_void::interface