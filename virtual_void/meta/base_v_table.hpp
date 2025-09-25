#pragma once

#include <typeinfo>
//#include <virtual_void/meta/class.hpp>
#include <virtual_void/utillities/VV_EXPORT.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {

class type_info;

struct base_v_table {
  template <typename CONCRETE>
  base_v_table(std::in_place_type_t<CONCRETE> concrete); //implementation see class.hpp

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(base_v_table) == from;
  }
  meta::type_info* type_info = nullptr;
  bool (*_is_derived_from)(const std::type_info&);
  base_v_table(auto unused)
      : _is_derived_from([](const std::type_info& from) {
          return static_is_derived_from(from);
        }) {};
};

template <typename CONCRETE>
base_v_table* base_v_table_imlpementation() {
  static base_v_table v_table{std::in_place_type<CONCRETE>};
  return &v_table;
}

}  // namespace virtual_void::meta
