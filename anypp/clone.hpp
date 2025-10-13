#pragma once

#include <anypp/clone_erased_data.hpp>
#include <anypp/query_v_table.hpp>
#include <anypp/meta_data.hpp>

namespace anypp {

template <is_any TO_ANYPP, is_erased_data ANY_FROM>
std::expected<TO_ANYPP, anypp::cast_error> clone_to(
    ANY_FROM const& vv_from, const meta_data& meta_data) {
  using vv_to_t = typename TO_ANYPP::erased_data_t;
  static_assert(is_erased_data<vv_to_t>);
  return query_v_table<TO_ANYPP>(meta_data).transform([&](auto v_table) {
    return TO_ANYPP{clone_to<vv_to_t>(vv_from, meta_data), v_table};
  });
}

template <is_any TO_ANYPP, is_any FROM_ANYPP>
auto clone_to(const FROM_ANYPP& from_interface) {
  return clone_to<TO_ANYPP>(get_erased_data(from_interface),
                                get_runtime(from_interface));
}

};  // namespace anypp