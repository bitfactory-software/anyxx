#pragma once

#include <typeindex>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {
class i_table;

template <typename CLASS>
constexpr i_table& get_i_table_of();

constexpr const std::type_info& get_type_info(i_table const&);

template <typename RUNTIME, typename TYPE>
RUNTIME& runtime();

template <typename RUNTIME, typename TYPE>
auto& runtime_implementation() {
  static RUNTIME runtime{std::in_place_type<TYPE>};
  return runtime;
}

}  // namespace virtual_void::meta
