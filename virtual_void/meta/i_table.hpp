#pragma once

#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/meta/archetype.hpp>
#include <virtual_void/meta/class.hpp>
#include <virtual_void/meta/interface.hpp>
#include <virtual_void/meta/table.hpp>

/*
namespace virtual_void::meta {

class i_table {
 public:
  using v_table_ptr = virtual_void::interface::base_v_table*;

 private:
  table<v_table_ptr, nullptr> table_;
 public:
  i_table() = default;

  constexpr void register_interface(int v_table_index, v_table_ptr v_table) {
    table_.register_target(v_table_index, v_table);
  }
  constexpr v_table_ptr at(int v_table_index) const {
    return table_.at(v_table_index);
  }
  constexpr auto size() const { return table_.size(); }
};  

template <typename CLASS>
struct i_table_implementation_of {
  static constexpr i_table& instance() {
    static i_table table{std::in_place_type<CLASS>};
    return table;
  }
};

template <typename CLASS>
struct i_table_of;

template <typename CLASS>
constexpr i_table& get_i_table_of() {
  return i_table_of<CLASS>::instance();
}


}  // namespace virtual_void::meta
*/