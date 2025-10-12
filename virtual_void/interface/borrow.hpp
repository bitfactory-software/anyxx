#pragma once

#undef interface

#include <virtual_void/data/borrow.hpp>
#include <virtual_void/interface/query_v_table.hpp>

namespace virtual_void {

template <is_interface TO_INTERFACE, data::is_erased_data VV_FROM>
  requires data::borrowable_from<typename TO_INTERFACE::erased_data_t, VV_FROM>
std::expected<TO_INTERFACE, virtual_void::runtime::cast_error> borrow_as(
    VV_FROM const& vv_from, const runtime::meta_data& meta_data) {
  using to = typename TO_INTERFACE::erased_data_t;
  return query_v_table<TO_INTERFACE>(meta_data).transform([&](auto v_table) {
    return TO_INTERFACE{data::borrow_as<to>(vv_from), v_table};
  });
}

template <is_interface TO_INTERFACE, is_interface FROM_INTERFACE>
  requires data::borrowable_from<typename TO_INTERFACE::erased_data_t,
                                 typename FROM_INTERFACE::erased_data_t>
auto borrow_as(FROM_INTERFACE const& from_interface) {
  return borrow_as<TO_INTERFACE>(get_erased_data(from_interface),
                                 get_runtime(from_interface));
}

};  // namespace virtual_void