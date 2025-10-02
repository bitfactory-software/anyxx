#pragma once

#include <virtual_void/data/clone.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/query.hpp>
#include <virtual_void/runtime/meta_data.hpp>

namespace virtual_void::interface {

template <is_interface TO_INTERFACE, data::is_erased_data VV_FROM>
std::expected<TO_INTERFACE, virtual_void::runtime::cast_error> clone_to(
    VV_FROM const& vv_from, const runtime::meta_data& meta_data) {
  using vv_to_t = typename TO_INTERFACE::erased_data_t;
  static_assert(data::is_erased_data<vv_to_t>);
  return query_v_table<TO_INTERFACE>(meta_data).transform([&](auto v_table) {
    return TO_INTERFACE{data::clone_to<vv_to_t>(vv_from, meta_data), v_table};
  });
}

template <is_interface TO_INTERFACE, data::is_erased_data VV_FROM>
auto clone_to(VV_FROM const& vv_from, runtime::base_v_table const* from) {
  return borrow_as<TO_INTERFACE>(vv_from, *from->meta_data);
}

template <is_interface TO_INTERFACE, is_interface FROM_INTERFACE>
auto clone_to(const FROM_INTERFACE& from_interface) {
  return clone_to<TO_INTERFACE>(get_erased_data(from_interface),
                                get_runtime(from_interface));
}

};  // namespace virtual_void::interface