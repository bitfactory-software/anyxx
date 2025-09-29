#pragma once

#include <virtual_void/data/erased_value.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

using value = erased_value;
}  // namespace virtual_void::data

namespace virtual_void {
template <>
struct erased_data_trait<data::value> : virtual_void_default_unerase {
  using void_t = void*;
  template <typename V>
  using typed_t = V;
  static constexpr bool is_constructibile_from_const = true;
  static void* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }
  template <typename V>
  static auto construct_from(V&& v) {
    return data::make_erased_value<typed_t<std::decay_t<V>>>(
        std::forward<V>(v));
  }
};

}  // namespace virtual_void

namespace virtual_void::data {
static_assert(is_erased_data<value>);
}  // namespace virtual_void::data