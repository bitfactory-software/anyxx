#pragma once

#include <bit_factory/anyxx/voidness.hpp>

namespace anyxx {

template <typename Proxy>
struct basic_proxy_trait {
  inline static constexpr bool is_weak = false;
  inline static constexpr bool is_lifetime_bound = false;
  inline static constexpr bool is_object = true;
  inline static constexpr bool allow_any_default_constructibile = false;

  template <typename VTable>
  static constexpr bool is_compatible_with_v_table() {
    return true;
  }

  template <typename Rep>
  using proxy_impl = Proxy;

  static void move_to(auto& to, [[maybe_unused]] auto, auto&& from,
                      [[maybe_unused]] auto) {
    to = std::move(from);
  }

  template <typename VTable>
  inline static constexpr bool can_copy_construct_from() {
    return true;
  }
  static void copy_construct_from(Proxy& to, [[maybe_unused]] void*,
                                  auto const& from, [[maybe_unused]] auto) {
    to = from;
  }

  static void destroy([[maybe_unused]] auto const& data,
                      [[maybe_unused]] void* v_table) {}
};

}  // namespace anyxx