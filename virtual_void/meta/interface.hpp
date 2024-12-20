#pragma once

#include <virtual_void/interface/base.hpp>
#include <virtual_void/meta/index_for_archetype.hpp>

namespace virtual_void::meta {

class interface_meta {
  // archetype index -> index for v_table in i_tables
  index_for_archetype index_;

 public:
  interface_meta() = default;
  interface_meta(interface_meta const&) = delete;

  int register_archetype(archetype_t& archetype) {
    return index_(archetype, &archetype_t::interface_count_);
  }
  int i_table_index(archetype_t& archetype) {
    return index_(archetype);
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