#pragma once

#include <virtual_void/data/move.hpp>
#include <virtual_void/interface/query_v_table.hpp>

namespace virtual_void::interface {

template <is_interface TO_INTERFACE, data::is_erased_data VV_FROM>
TO_INTERFACE move_to(VV_FROM&& vv_from,
                     const runtime::meta_data& get_meta_data) {
  using vv_to_t = typename TO_INTERFACE::erased_data_t;
  static_assert(data::is_erased_data<vv_to_t>);
  auto v_table = query_v_table<TO_INTERFACE>(get_meta_data);
  return TO_INTERFACE{data::move_to<vv_to_t>(std::move(vv_from)), *v_table};
}

template <is_interface TO_INTERFACE, is_interface FROM_INTERFACE>
TO_INTERFACE move_to(FROM_INTERFACE&& from_interface) {
  return move_to<TO_INTERFACE>(move_erased_data(std::move(from_interface)),
                               get_runtime(from_interface));
}

};  // namespace virtual_void::interface