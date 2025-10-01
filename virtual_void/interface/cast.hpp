#pragma once

#undef interface

#include <ranges>
#include <virtual_void/data/cast.hpp>
#include <virtual_void/data/copy_convert.hpp>
#include <virtual_void/data/move_convert.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/runtime/meta_data.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::interface {

template <typename TO_INTERFACE>
auto find_v_table(const runtime::meta_data& meta_data)
    -> std::expected<typename TO_INTERFACE::v_table_t*,
                     virtual_void::runtime::cast_error> {
  using v_table_t = typename TO_INTERFACE::v_table_t;
  return meta_data.get_v_table(typeid(v_table_t)).transform([](auto v_table) {
    return static_cast<v_table_t*>(v_table);
  });
}

template <typename TO_INTERFACE>
auto find_v_table(runtime::base_v_table* from) -> TO_INTERFACE::v_table_t* {
  return find_v_table<TO_INTERFACE>(*from->meta_data);
}

template <is_interface TO_INTERFACE, data::is_erased_data VV_FROM>
  requires data::cast_convertable_from<typename TO_INTERFACE::erased_data_t,
                                       VV_FROM>
std::expected<TO_INTERFACE, virtual_void::runtime::cast_error>
dynamic_interface_cast(VV_FROM const& vv_from,
                       const runtime::meta_data& meta_data) {
  using to = typename TO_INTERFACE::erased_data_t;
  return find_v_table<TO_INTERFACE>(meta_data).transform([&](auto v_table) {
    return TO_INTERFACE{data::cast_to<to>(vv_from, meta_data), v_table};
  });
}

template <typename TO_INTERFACE, data::is_erased_data VV_FROM>
auto dynamic_interface_cast(VV_FROM const& vv_from,
                            runtime::base_v_table const* from) {
  return dynamic_interface_cast<TO_INTERFACE>(vv_from, *from->meta_data);
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
auto dynamic_interface_cast(const FROM_INTERFACE& from_interface) {
  return dynamic_interface_clone_cast<TO_INTERFACE>(
      get_erased_data(from_interface), get_runtime(from_interface));
}

template <is_interface TO_INTERFACE, data::is_erased_data VV_FROM>
std::expected<TO_INTERFACE, virtual_void::runtime::cast_error>
dynamic_interface_clone_cast(VV_FROM const& vv_from,
                             const runtime::meta_data& meta_data) {
  using vv_to_t = typename TO_INTERFACE::erased_data_t;
  static_assert(data::is_erased_data<vv_to_t>);
  return find_v_table<TO_INTERFACE>(meta_data).transform([&](auto v_table) {
    return TO_INTERFACE{copy_convert_to<vv_to_t>(vv_from, meta_data), v_table};
  });
}

template <typename TO_INTERFACE, data::is_erased_data VV_FROM>
auto dynamic_interface_clone_cast(VV_FROM const& vv_from,
                                  runtime::base_v_table const* from) {
  return dynamic_interface_cast<TO_INTERFACE>(vv_from, *from->meta_data);
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
auto dynamic_interface_clone_cast(const FROM_INTERFACE& from_interface) {
  return dynamic_interface_clone_cast<TO_INTERFACE>(
      get_erased_data(from_interface), get_runtime(from_interface));
}

template <typename TO_INTERFACE, data::is_erased_data VV_FROM>
TO_INTERFACE move_to_interface(VV_FROM&& vv_from,
                               const runtime::meta_data& get_meta_data) {
  using vv_to_t = typename TO_INTERFACE::erased_data_t;
  static_assert(data::is_erased_data<vv_to_t>);
  auto v_table = find_v_table<TO_INTERFACE>(get_meta_data);
  return TO_INTERFACE{move_convert_to<vv_to_t>(std::move(vv_from)), *v_table};
}

template <typename TO_INTERFACE, typename FROM_INTERFACE>
TO_INTERFACE move_to_interface(FROM_INTERFACE&& from_interface) {
  return move_to_interface<TO_INTERFACE>(
      move_erased_data(std::move(from_interface)), get_runtime(from_interface));
}

};  // namespace virtual_void::interface