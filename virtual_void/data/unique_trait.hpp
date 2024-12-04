#pragma once

#include <virtual_void/data/decorated_data.hpp>
#include <virtual_void/data/make_unique_decorated_data.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void {

template <typename META>
struct unique_trait : virtual_void_default_unerase {
  using meta_t = META;
  using void_t = void*;
  template <typename V>
  using typed_t = data::decorated_data<V, META>;
  static constexpr bool is_constructibile_from_const = true;

  static void* value(const auto& ptr) { return ptr->value(); }
  static auto meta(const auto& ptr) { return ptr->get_meta(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename V>
  static auto construct_from(V&& v) {
    return data::make_unique_decorated_data<typed_t<std::decay_t<V>>>(std::forward<V>(v));
  }
  template <typename V, typename... ARGS>
  static auto construct_in_place(std::in_place_type_t<V>, ARGS&&... args) {
    return data::make_unique_decorated_data<typed_t<V>>(std::forward<ARGS>(args)...);
  }
};

}  // namespace virtual_void