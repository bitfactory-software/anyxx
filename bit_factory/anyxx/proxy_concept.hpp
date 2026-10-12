#pragma once

#include <bit_factory/anyxx/voidness.hpp>

namespace anyxx {

template <typename Proxy>
struct proxy_trait;

/// Requirements for a proxy type
template <typename E>
concept is_proxy = requires(E e) {
  typename proxy_trait<E>::void_t;
  typename proxy_trait<E>::static_dispatch_t;
  { proxy_trait<E>::is_constructibile_from_const } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_owner } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_weak } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_lifetime_bound } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_object } -> std::convertible_to<bool>;
  {
    proxy_trait<E>::allow_any_default_constructibile
  } -> std::convertible_to<bool>;

  // illustrated:
  // requires requires(v_table* required_v_table) {
  //   {
  //     proxy_trait<E>::get_proxy_ptr_in(e, required_v_table)
  //   } -> std::convertible_to<typename proxy_trait<E>::void_t>;
  // };
  // requires !proxy_trait<E>::is_owner ||
  //              requires(mutable_void from_data, a _v_table* v_table) {
  //                {
  //                  proxy_trait<E>::clone_from(from_data, v_table)
  //                };
  //              };
};

template <typename ProxyTrait, typename VTable>
concept can_copy_construct_from =
    ProxyTrait::template can_copy_construct_from<VTable>();

/// Requirements for a dynamic (i.e., type-erased) Proxy
template <typename Proxy>
concept is_dyn =
is_proxy<Proxy> && voidness<typename proxy_trait<Proxy>::static_dispatch_t>;

}  // namespace anyxx