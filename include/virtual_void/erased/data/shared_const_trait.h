#pragma once

#include <stdexcept>
#include <type_traits>

#include "../data/holder.h"
#include "../data/shared_const_ptr.h"
#include "../virtual_void.h"

namespace virtual_void::erased {

template <typename META>
struct shared_const_trait
    : data_trait_base<data::shared_const_ptr<data::base<META>>> {
  using void_t = void const*;
  using ptr_t = data::shared_const_ptr<data::base<META>>;
  template <typename V>
  using typed_t = data::holder<V, META>;

  static void const* value(const auto& ptr) { return ptr->value(); }
  static auto meta(const auto& ptr) { return ptr->get_meta(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename V>
  static auto construct_from(V&& v) {
    return data::make_shared_const<typed_t<std::decay_t<V>>>(
        std::forward<V>(v));
  }
  template <typename V, typename... ARGS>
  static auto construct_in_place(std::in_place_type_t<V>, ARGS&&... args) {
    return data::make_shared_const<typed_t<V>>(std::forward<ARGS>(args)...);
  }
};

}  // namespace virtual_void::erased