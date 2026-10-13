#pragma once

#include <bit_factory/anyxx/proxies/proxy_concept.hpp>

namespace anyxx {

template <typename T>
concept has_v_table = requires { typename T::v_table_t; };

template <typename Trait, is_proxy Proxy = typename Trait::default_proxy_t>
class any;

template <typename I>
concept is_any_impl = is_proxy_holder_impl<I> &&
                      requires(I::proxy_t ed) { typename I::v_table_t; };
template <typename I>
concept is_any = is_any_impl<std::decay_t<I>>;

template <class E>
concept is_typed_any = is_any<E> && requires(E e) {
  typename E::proxy_trait_t;
  typename E::value_t;
  { E::is_const } -> std::convertible_to<bool>;
};

template <typename ConstructedWith, typename Proxy, typename Any>
concept constructibile_for =
(proxy_trait<Proxy>::template is_constructibile_from<
    ConstructedWith>::value) ||
    (erased_constructibile_for<ConstructedWith, Proxy, Any> &&
        !is_proxy_holder<ConstructedWith> &&
        !is_typed_any<std::remove_cvref_t<ConstructedWith>>);


template <typename DerivedAny, typename BaseAny>
constexpr bool is_any_derived_from_v = false;

template <typename DerivedAny, typename BaseAny>
concept is_any_derived_from = is_any_derived_from_v<DerivedAny, BaseAny>;

}  // namespace anyxx