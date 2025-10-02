#pragma once

#undef interface

#include <virtual_void/interface/base.hpp>
#include <virtual_void/runtime/meta_data.hpp>

namespace virtual_void::interface {

template <is_interface TO_INTERFACE>
auto query_v_table(const runtime::meta_data& meta_data)
    -> std::expected<typename TO_INTERFACE::v_table_t*,
                     virtual_void::runtime::cast_error> {
  using v_table_t = typename TO_INTERFACE::v_table_t;
  return meta_data.get_v_table(typeid(v_table_t)).transform([](auto v_table) {
    return static_cast<v_table_t*>(v_table);
  });
}

template <typename TO_INTERFACE>
auto query_v_table(runtime::base_v_table* from) {
  return find_v_table<TO_INTERFACE>(*from->meta_data);
}

template <is_interface TO_INTERFACE, data::is_erased_data VV_FROM>
  requires data::borrowable_from<typename TO_INTERFACE::erased_data_t, VV_FROM>
std::expected<TO_INTERFACE, virtual_void::runtime::cast_error> query_interface(
    VV_FROM const& vv_from, const runtime::meta_data& meta_data) {
  using to = typename TO_INTERFACE::erased_data_t;
  return query_v_table<TO_INTERFACE>(meta_data).transform([&](auto v_table) {
    return TO_INTERFACE{data::borrow_as<to>(vv_from), v_table};
  });
}

template <is_interface TO_INTERFACE, data::is_erased_data VV_FROM>
  requires data::borrowable_from<typename TO_INTERFACE::erased_data_t, VV_FROM>
auto query_interface(VV_FROM const& vv_from,
                     runtime::base_v_table const* from) {
  return query_interface<TO_INTERFACE>(vv_from, *from->meta_data);
}

template <is_interface TO_INTERFACE, is_interface FROM_INTERFACE>
  requires data::borrowable_from<typename TO_INTERFACE::erased_data_t,
                               typename FROM_INTERFACE::erased_data_t>
auto query_interface(FROM_INTERFACE const& from_interface) {
  return query_interface<TO_INTERFACE>(get_erased_data(from_interface),
                                       get_runtime(from_interface));
}

};  // namespace virtual_void::interface