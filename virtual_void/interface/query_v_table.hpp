#pragma once

#undef interface

#include <virtual_void/interface/base.hpp>
#include <virtual_void/runtime/meta_data.hpp>

namespace virtual_void {

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

};  // namespace virtual_void