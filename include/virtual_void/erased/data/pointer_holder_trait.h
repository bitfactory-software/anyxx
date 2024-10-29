#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../erased/virtual_void.h"

namespace virtual_void::erased {

template <typename DATA>
struct pointer_holder_trait : virtual_void_trait_base<DATA> {
  using void_t = DATA::void_t;
  static void_t value(const auto& ptr) { return ptr.value(); }
  static auto meta(const auto& ptr) { return ptr.get_meta(); }
  static bool has_value(const auto& ptr) {
    return static_cast<bool>(ptr.value());
  }
  template <typename V>
  static auto construct_from(V& v) {
    return DATA(v);
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires DATA::is_const
  {
    return DATA(v);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, ARG& arg) {
    return DATA(arg);
  }
  template <typename V, typename ARG>
  static auto construct_in_place(std::in_place_type_t<V>, const ARG& arg)
    requires DATA::is_const
  {
    return DATA(arg);
  }
  template <typename FROM>
  auto operator()(FROM& from) {
    return virtual_void<DATA>(from);
  }  // for migration to lifteime_handle! delete after migration!
  template <typename FROM>
  auto operator()(const FROM& from)
    requires DATA::is_const
  {
    return virtual_void<DATA>(from);
  }  // for migration to lifteime_handle! delete after migration!
};

}  // namespace virtual_void::erased
