#pragma once

#include <anypp/move_erased_data.hpp>
#include <anypp/query_v_table.hpp>

namespace anypp {

template <is_any TO_ANYPP, is_erased_data ANY_FROM>
TO_ANYPP move_to(ANY_FROM&& vv_from,
                     const meta_data& get_meta_data) {
  using vv_to_t = typename TO_ANYPP::erased_data_t;
  static_assert(is_erased_data<vv_to_t>);
  auto v_table = query_v_table<TO_ANYPP>(get_meta_data);
  return TO_ANYPP{move_to<vv_to_t>(std::move(vv_from)), *v_table};
}

template <is_any TO_ANYPP, is_any FROM_ANYPP>
TO_ANYPP move_to(FROM_ANYPP&& from_interface) {
  return move_to<TO_ANYPP>(move_erased_data(std::move(from_interface)),
                               get_runtime(from_interface));
}

};  // namespace anypp