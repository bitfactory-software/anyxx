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

template <typename I>
concept is_proxy_holder_impl = requires(I i) {
  typename I::proxy_t;
  typename I::proxy_trait_t;
};
template <typename I>
concept is_proxy_holder = is_proxy_holder_impl<std::decay_t<I>>;

template <typename ConstructedWith, typename Proxy, typename BASE>
concept erased_constructibile_for =
    (!std::derived_from<std::remove_cvref_t<ConstructedWith>, BASE> &&
     !is_proxy<std::remove_cvref_t<ConstructedWith>> &&
     (!std::is_const_v<std::remove_reference_t<ConstructedWith>> ||
      proxy_trait<Proxy>::is_constructibile_from_const));

template <is_proxy Data>
using data_void = proxy_trait<Data>::void_t;

template <typename Proxy>
concept is_const_data = is_proxy<Proxy> && is_const_void<data_void<Proxy>>;

template <typename Proxy>
concept is_object_proxy = is_proxy<Proxy> && proxy_trait<Proxy>::is_object;

template <typename Proxy>
concept is_weak_data = is_proxy<Proxy> && proxy_trait<Proxy>::is_weak;

template <typename Proxy>
concept is_lifetime_bound =
    is_proxy<Proxy> && proxy_trait<Proxy>::is_lifetime_bound;

template <bool ToIsConst, bool FromIsConst, bool FromIsWeak>
concept const_correct_move_to_from =
    !FromIsWeak && ((ToIsConst == FromIsConst) || !FromIsConst);

constexpr inline bool is_const_correct_call_for_proxy_and_self(
    bool call_is_const, bool proxy_is_const, bool self_is_const, bool exact) {
  if (call_is_const) {
    if (exact) {
      return (call_is_const == proxy_is_const);
    } else {
      return true;
    }
  }

  if (proxy_is_const || self_is_const) return false;

  if (exact)
    return (call_is_const == proxy_is_const);
  else
    return true;
}

template <typename CALL, typename Proxy, bool SelfIsConst, bool Exact>
concept const_correct_call_for_proxy_and_self =
    is_object_proxy<Proxy> && !is_weak_data<Proxy> && voidness<CALL> &&
    is_proxy<Proxy> &&
    is_const_correct_call_for_proxy_and_self(
        is_const_void<CALL>, is_const_data<Proxy>, SelfIsConst, Exact);

template <is_proxy Proxy, typename From>
Proxy erased(From&& from) {
  return proxy_trait<Proxy>::erase(std::forward<From>(from));
}

template <is_proxy Proxy, typename ConstructedWith>
using unerased =
    proxy_trait<Proxy>::template unerased<std::decay_t<ConstructedWith>>;

template <is_proxy Proxy>
void const* get_proxy_ptr(Proxy const& vv, auto v_table)
  requires std::same_as<void const*, typename proxy_trait<Proxy>::void_t>
{
  return proxy_trait<Proxy>::get_proxy_ptr_in(vv, v_table);
}
template <is_proxy Proxy>
void* get_proxy_ptr(Proxy const& vv, auto v_table)
  requires std::same_as<void*, typename proxy_trait<Proxy>::void_t>
{
  return proxy_trait<Proxy>::get_proxy_ptr_in(vv, v_table);
}
template <is_proxy Proxy>
auto get_proxy_ptr(Proxy& vv, auto v_table) {
  return proxy_trait<Proxy>::get_proxy_ptr_in(vv, v_table);
}

}  // namespace anyxx