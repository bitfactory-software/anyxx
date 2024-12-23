#pragma once

#include <virtual_void/meta/class.hpp>
#include <virtual_void/utillities/unnamed__.hpp>

namespace virtual_void::meta {

template <typename V_TABLE>
int archetype_index_in_v_table(archetype& archetype);
template <>
inline int archetype_index_in_v_table<base_v_table>(archetype& archetype) {
  return -1;
}
template <typename V_TABLE>
int archetype_index_in_v_table(archetype& archetype) {
  using base_t = V_TABLE::v_table_base_t;
  auto index = archetype_index_in_v_table<base_t>(archetype);
  auto& interface_meta = runtime<meta::interface, V_TABLE>();
  if (index < 0)
    return interface_meta.register_archetype(archetype);
  else
    return interface_meta.register_archetype_at(archetype, index);
}

template <typename CLASS, typename V_TABLE>
struct class_implements_interface{
  constexpr class_implements_interface() {
    auto& type_info = runtime<meta::type_info, CLASS>();
    auto& i_table = type_info.get_i_table();
    auto& archetype = type_info.get_archetype();
    auto i_table_idx = archetype_index_in_v_table<V_TABLE>(archetype);
    using uneraser = static_cast_uneraser<CLASS>;
    auto v_table_ptr = V_TABLE::template imlpementation<uneraser>();
    i_table.register_target(i_table_idx, v_table_ptr);
  }
};

}  // namespace virtual_void::meta

#define VV_CLASS_IMPLEMENTS_INTERFACE(class_, interface_) \
virtual_void::meta::class_implements_interface<class_, interface_##_v_table> __;