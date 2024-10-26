#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../erased/virtual_void.h"

namespace virtual_void::erased {

template <typename DATA_PTR>
struct observer_trait : data_trait_base<DATA_PTR> {
  using void_t = DATA_PTR::void_t;
  static void_t value(const auto& ptr) { return ptr.value(); }
  static auto meta(const auto& ptr) { return ptr.get_meta(); }
  static bool has_value(const auto& ptr) {
    return static_cast<bool>(ptr.value());
  }
  template <typename V>
  static auto construct_from(V& v) {
    return DATA_PTR(v);
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires DATA_PTR::is_const
  {
    return DATA_PTR(v);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, ARG& arg) {
    return DATA_PTR(arg);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, const ARG& arg)
    requires DATA_PTR::is_const
  {
    return DATA_PTR(arg);
  }
  template <typename FROM>
  auto operator()(FROM& from) {
    return virtual_void<DATA_PTR>(from);
  }  // for migration to lifteime_handle! delete after migration!
  template <typename FROM>
  auto operator()(const FROM& from)
    requires DATA_PTR::is_const
  {
    return virtual_void<DATA_PTR>(from);
  }  // for migration to lifteime_handle! delete after migration!
};

}  // namespace virtual_void::erased
