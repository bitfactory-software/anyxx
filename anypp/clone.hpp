#pragma once

#include <anypp/clone_erased_data.hpp>
#include <anypp/query_v_table.hpp>
#include <anypp/meta_data.hpp>

namespace virtual_void {

template <is_any TO_INTERFACE, is_erased_data VV_FROM>
std::expected<TO_INTERFACE, virtual_void::cast_error> clone_to(
    VV_FROM const& vv_from, const meta_data& meta_data) {
  using vv_to_t = typename TO_INTERFACE::erased_data_t;
  static_assert(is_erased_data<vv_to_t>);
  return query_v_table<TO_INTERFACE>(meta_data).transform([&](auto v_table) {
    return TO_INTERFACE{clone_to<vv_to_t>(vv_from, meta_data), v_table};
  });
}

template <is_any TO_INTERFACE, is_any FROM_INTERFACE>
auto clone_to(const FROM_INTERFACE& from_interface) {
  return clone_to<TO_INTERFACE>(get_erased_data(from_interface),
                                get_runtime(from_interface));
}

};  // namespace virtual_void