#pragma once

#include <bit_factory/anyxx.hpp>
#include <variant>

namespace anyxx {

/// A Proxy for mixing std::variant and type erasure
/// Use this when some, at compile time, known types are dispatched in a hot
/// path. Put these types plus an any_ (with the specific \ref trait and a
/// dynamic proxy) in a std::variant. This variant is then used with a \ref
/// using_ proxy and the same \ref trait as before. See \ref make_vany
/// So the dispatch for the types in the variant is done internally with a
/// std::visit, and all other types are dispatched via their v-Table. Now you
/// know where the Any++ logo has its origin.
/// \ingroup proxies
template <template<typename...> typename Trait, typename... Types>
using vany_variant = std::variant<any<Trait>, Types...>;

/// A factory type to direct get the any_ for the vany
template <template<typename...> typename Trait, typename... Types>
using make_vany = any_<using_<vany_variant<Trait, Types...>>, Trait>;

template <typename VanyProxy>
struct vany_variant_trait {
  using vany_variant = typename VanyProxy::value_t;
  template <typename... Types>
  struct concrete_variant_impl;
  template <typename First, typename... Types>
  struct concrete_variant_impl<std::variant<First, Types...>> {
    using type = std::variant<Types...>;
  };
  using concrete_variant = typename concrete_variant_impl<vany_variant>::type;
  using any_in_variant = typename std::variant_alternative_t<0, vany_variant>;
};

template <typename Vany>
struct vany_type_trait {
  using vany = Vany;
  using vany_proxy_t = typename Vany::proxy_t;
  using concrete_variant =
      typename vany_variant_trait<vany_proxy_t>::concrete_variant;
  using any_in_variant =
      typename vany_variant_trait<vany_proxy_t>::any_in_variant;
};

template <template<typename...> typename Trait, typename... Types>
struct proxy_trait<using_<vany_variant<Trait, Types...>>>
    : basic_proxy_trait<using_<vany_variant<Trait, Types...>>> {
  using vany_variant_t = vany_variant<Trait, Types...>;
  using vany_proxy_t = typename Trait<>::default_proxy_t;
  using void_t = typename proxy_trait<vany_proxy_t>::void_t;
  using static_dispatch_t = vany_variant_t;
  static constexpr bool is_constructibile_from_const =
      proxy_trait<vany_proxy_t>::is_constructibile_from_const;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value =
        std::is_constructible_v<vany_variant_t, ConstructedWith>;
  };
  static constexpr bool is_owner = proxy_trait<vany_proxy_t>::is_owner;
  static constexpr bool is_weak =
      proxy_trait<vany_proxy_t>::is_weak;  // cppcheck-suppress
  // duplInheritedMember
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] auto* v_table) {
    return void_t{};
  }

  static auto get_proxy_ptr_in(auto& val, [[maybe_unused]] auto* v_table) {
    return val;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static vany_variant_t construct_in_place(V&& v) {
    return using_<vany_variant_t>{std::forward<V>(v)};
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    return using_<vany_variant_t>{T{std::forward<Args>(args)...}};
  }
  template <typename Vx>
  static auto erase(Vx&& v) {
    return using_<vany_variant_t>{std::forward<Vx>(v)};
  }
};

}  // namespace anyxx