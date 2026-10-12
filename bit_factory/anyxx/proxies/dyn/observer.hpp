#pragma once

#include <bit_factory/anyxx/proxies/basic_proxy_trait.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/voidness.hpp>

namespace anyxx {

template <voidness Voidness>
struct observer_trait : basic_proxy_trait<Voidness> {
  using void_t = Voidness;
  using static_dispatch_t = void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  static constexpr bool is_constructibile_from_const = is_const;
  static constexpr bool is_lifetime_bound = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = false;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    return void_t{};
  }
  static void move_to(Voidness& to, [[maybe_unused]] auto, Voidness from,
                      [[maybe_unused]] auto) {
    to = from;
  }

  static Voidness get_proxy_ptr_in(const auto& ptr,
                                   [[maybe_unused]] auto* v_table) {
    return ptr;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static auto construct_in_place(V&&) {
    static_assert(false);
    return nullptr;
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    static_assert(false);
    return nullptr;
  }
  template <typename V>
  static auto erase(V& v) {
    static_assert(!std::is_const_v<std::remove_reference_t<V>>);
    return static_cast<Voidness>(&v);
  }
  template <typename V>
  static auto erase(const V& v)
    requires(is_const)
  {
    return static_cast<Voidness>(&v);
  }
};

template <>
struct proxy_trait<cref> : observer_trait<cref> {};
template <>
struct proxy_trait<mutref> : observer_trait<mutref> {};

static_assert(proxy_trait<cref>::is_const);
static_assert(!proxy_trait<mutref>::is_const);
static_assert(is_proxy<cref>);
static_assert(is_proxy<mutref>);
static_assert(is_proxy<mutref>);
static_assert(is_proxy<cref>);


}  // namespace anyxx