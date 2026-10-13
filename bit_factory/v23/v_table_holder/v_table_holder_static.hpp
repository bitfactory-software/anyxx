#pragma once

#include <bit_factory/v23/v_table_holder/v_table_holder_forward.hpp>

namespace anyxx {

template <typename Proxy, typename Trait>
using proxy_model_map_t =
typename Trait::template model_map<static_dispatch_t<Proxy>>;

template <typename Proxy, typename Trait>
using proxy_deduced_type_t =
typename proxy_model_map_t<Proxy, Trait>::deduced_type;

template <typename Proxy, typename Trait>
  requires(!is_dyn<Proxy>)
struct v_table_holder<Proxy, Trait> : proxy_deduced_type_t<Proxy, Trait> {
  struct v_table_t {};

  v_table_holder() = default;
  explicit v_table_holder(v_table_t*) {}
  static void set_v_table_ptr(auto) {}
  static auto get_v_table_ptr() { return nullptr; }
  template <typename...>
  static void init_v_table() {}
  static auto release_v_table() { return nullptr; }
};


}  // namespace anyxx