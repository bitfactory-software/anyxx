#pragma once

#include <typeindex>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::meta {
class i_table;
struct base_v_table;

constexpr const std::type_info& get_type_info(i_table const&);

class runtime_t;

template <typename TYPE>
runtime_t& runtime();

template <typename TYPE>
auto& runtime_implementation() {
  static runtime_t runtime{std::in_place_type<TYPE>};
  return runtime;
}

class type_info;

}  // namespace virtual_void::meta

#define VV_RUNTIME(export_, ...)         \
  template <>                            \
  export_ virtual_void::meta::runtime_t& \
  virtual_void::meta::runtime<__VA_ARGS__>();

#define VV_RUNTIME_IMPEMENTATION(...)                                         \
  template <>                                                                 \
  virtual_void::meta::runtime_t& virtual_void::meta::runtime<__VA_ARGS__>() { \
    return runtime_implementation<__VA_ARGS__>();                             \
  }

#define VV_RUNTIME_STATIC(...) \
  VV_RUNTIME(, __VA_ARGS__)    \
  VV_RUNTIME_IMPEMENTATION(__VA_ARGS__)\
