#pragma once

#include <anypp/borrow_erased_data.hpp>
#include <anypp/query_v_table.hpp>

namespace anypp {

template <is_any TO_ANYPP, is_erased_data VV_FROM>
  requires borrowable_from<typename TO_ANYPP::erased_data_t, VV_FROM>
std::expected<TO_ANYPP, anypp::cast_error> borrow_as(
    VV_FROM const& vv_from, const meta_data& meta_data) {
  using to = typename TO_ANYPP::erased_data_t;
  return query_v_table<TO_ANYPP>(meta_data).transform([&](auto v_table) {
    return TO_ANYPP{borrow_as<to>(vv_from), v_table};
  });
}

template <is_any TO_ANYPP, is_any FROM_ANYPP>
  requires borrowable_from<typename TO_ANYPP::erased_data_t,
                                 typename FROM_ANYPP::erased_data_t>
auto borrow_as(FROM_ANYPP const& from_interface) {
  return borrow_as<TO_ANYPP>(get_erased_data(from_interface),
                                 get_runtime(from_interface));
}

};  // namespace anypp