#pragma once

#include <bit_factory/v23/anys/range/iterators.hpp>
#include <bit_factory/anyxx.hpp>

namespace anyxx {

TRAIT_TEMPLATE_(((AnyIterator)), range, dynamic_copyable, (),
                (ANY_FN(AnyIterator, begin, (), const),
                 ANY_FN(AnyIterator, end, (), const)))

template <typename AnyIterator, typename Proxy = anyxx::cref>
using any_range = any<Proxy, range, AnyIterator>;

template <typename ValueType, typename Reference = ValueType,
          typename Proxy = anyxx::cref>
using any_forward_range =
    any_range<any_forward_iterator<ValueType, Reference>, Proxy>;

using any_self_forward_range = any_forward_range<self, self>;
template <typename A>
concept is_any_self_forward_range =
    is_any<A> && std::ranges::forward_range<A> &&
    std::same_as<std::ranges::range_value_t<A>, self>;
static_assert(is_any_self_forward_range<any_self_forward_range>);

template <typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_sig_map<AnyForwardRange const&> : translate_sig_map<self> {
  template <typename AnyValue>
  using v_table_param =
      any_forward_range<AnyValue, AnyValue,
                        typename AnyForwardRange::proxy_t> const&;
  template <typename Model>
  using concept_arg = anyxx::any_forward_range<Model, Model, anyxx::cref>;
};

template <typename Concrete, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct v_table_to_map<Concrete, AnyForwardRange const&> {
  static auto forward(auto const& any_range) {
    return std::views::transform(any_range, [](auto const& any) {
      return *unerase_cast<Concrete>(any);
    });
  }
};
//
template <typename Traited, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct forward_trait_to_map<Traited, AnyForwardRange const&> {
  static auto forward(auto any_range) {
    return std::views::transform(any_range,
                                 []<typename T>(T const& any) -> Traited {
                                   if constexpr (is_any<T>) {
                                     if constexpr (T::is_dyn) {
                                       return *unerase_cast<Traited>(any);
                                     } else {
                                       return get_proxy_value(any);
                                     }
                                   } else {
                                     return any;
                                   }
                                 });
  }
};

static_assert(
    any_forward_iterator<self, self>::v_table_t::val_is_nullable::value ==
    true);
static_assert(
    std::same_as<any_forward_iterator<self, self>::v_table_t::any_value_t,
                 any<nullable_val<iterator_val_proxy_size>, forward_iterator,
                     self, self>>);
static_assert(
    std::same_as<random_access_iterator<self, self>::v_table_t::any_value_t,
                 any<nullable_val<iterator_val_proxy_size>,
                     random_access_iterator, self, self>>);
static_assert(std::forward_iterator<any_forward_iterator<self, self>>);
static_assert(std::ranges::forward_range<any_forward_range<self, self>>);

template <template <typename...> typename Trait, typename... Ts>
auto trait_range_as(auto&& r) {
  return std::forward<decltype(r)>(r) |
         std::views::transform(
             [&](auto const& x) { return trait_as<Trait, Ts...>(x); });
}

}  // namespace anyxx