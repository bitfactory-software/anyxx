#pragma once

#undef interface

#include <anypp/any_base.hpp>
#include <anypp/meta_data.hpp>

namespace virtual_void {

template <is_any TO_ANYPP>
auto query_v_table(const meta_data& meta_data)
    -> std::expected<typename TO_ANYPP::v_table_t*,
                     virtual_void::cast_error> {
  using v_table_t = typename TO_ANYPP::v_table_t;
  return meta_data.get_v_table(typeid(v_table_t)).transform([](auto v_table) {
    return static_cast<v_table_t*>(v_table);
  });
}

template <typename TO_ANYPP>
auto query_v_table(any_base_v_table* from) {
  return find_v_table<TO_ANYPP>(*from->meta_data);
}

};  // namespace virtual_void