#pragma once

#include <typeindex>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {
class i_table;
struct base_v_table;

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

#define VV_RUNTIME(export_, runtime_, ...) \
  template <>                                \
  export_ virtual_void::meta::runtime_&      \
  virtual_void::meta::runtime<virtual_void::meta::runtime_, __VA_ARGS__>();

#define VV_RUNTIME_IMPEMENTATION(runtime_, ...)                      \
  template <>                                                          \
  virtual_void::meta::runtime_&                                        \
  virtual_void::meta::runtime<virtual_void::meta::runtime_, __VA_ARGS__>() { \
    return runtime_implementation<runtime_, __VA_ARGS__>();                  \
  }

#define VV_RUNTIME_STATIC(runtime_, ...) \
  VV_RUNTIME(, runtime_, __VA_ARGS__)            \
  VV_RUNTIME_IMPEMENTATION(runtime_, __VA_ARGS__)\
