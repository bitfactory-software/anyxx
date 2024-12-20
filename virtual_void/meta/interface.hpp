#pragma once

#include <virtual_void/interface/base.hpp>
#include <virtual_void/meta/index_for_archetype.hpp>

namespace virtual_void::meta {

class interface_meta {
  // archetype index -> index for v_table in i_tables
  index_for_archetype index_for_v_table_in_i_table_;

 public:
  interface_meta() = default;
  interface_meta(interface_meta const&) = delete;

  int register_archetype(archetype_t& archetype) {
    auto index = archetype.interface_count_++;
    index_for_v_table_in_i_table_.register_archetype(
        archetype.get_archetype_index(), index);
    return index;
  }
  int i_table_index(archetype_t& archetype) {
    auto archetype_index = archetype.get_archetype_index();
    return index_for_v_table_in_i_table_.at(archetype_index);
  }
};

template <typename V_TABLE>
interface_meta& interface_meta_for();

template <typename V_TABLE>
interface_meta& interface_meta_for_implementation() {
  static interface_meta meta_;
  return meta_;
}

}  // namespace virtual_void::meta