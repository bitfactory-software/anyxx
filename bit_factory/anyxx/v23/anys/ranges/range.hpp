#pragma once

#include <bit_factory/anyxx/v23/anys/ranges/iterators.hpp>

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

template <template <typename...> typename Trait, typename... Ts>
auto trait_range_as(auto&& r) {
  return std::forward<decltype(r)>(r) |
         std::views::transform(
             [&](auto const& x) { return using_as<Trait, Ts...>(x); });
}

}  // namespace anyxx