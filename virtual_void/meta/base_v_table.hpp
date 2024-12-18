#pragma once

#include <typeinfo>
#include <virtual_void/utillities/VV_EXPORT.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {

struct base_v_table {
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(base_v_table) == from;
  }
  bool (*_is_derived_from)(const std::type_info&);
  base_v_table(auto unused)
      : _is_derived_from([](const std::type_info& from) {
          return static_is_derived_from(from);
        }){};
};

VV_EXPORT base_v_table* base_v_table_imlpementation();
VV_EXPORT int next_i_table_index_value();

template <typename V_TABLE>
int i_table_index_implemntation() {
  static int i = next_i_table_index_value();
  return i;
}
template <typename V_TABLE>
int i_table_index();

}  // namespace virtual_void::meta
