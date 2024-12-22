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

class type_info;

}  // namespace virtual_void::meta

#define VV_RUNTIME(export_, runtime_, from_) \
  template <>                                \
  export_ virtual_void::meta::runtime_&      \
  virtual_void::meta::runtime<virtual_void::meta::runtime_, from_>();

#define VV_RUNTIME_IMPEMENTATION(runtime_, from_)                      \
  template <>                                                          \
  virtual_void::meta::runtime_&                                        \
  virtual_void::meta::runtime<virtual_void::meta::runtime_, from_>() { \
    return runtime_implementation<runtime_, from_>();                  \
  }

#define VV_RUNTIME_STATIC(runtime_, from_) \
  VV_RUNTIME(, runtime_, from_)            \
  VV_RUNTIME_IMPEMENTATION(runtime_, from_)\
