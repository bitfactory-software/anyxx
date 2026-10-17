#pragma once

#include <bit_factory/anyxx/v23/anys/ranges/iterators.hpp>
#include <ranges>

namespace anyxx {

struct sentinel_is_nullable {};
struct sentinel_val_size {
  static constexpr std::size_t value = iterator_val_proxy_size;
};

template <typename Sentinel, typename Iterator>
struct sentinel_for {
  using iterator_type = Iterator;
  Sentinel value;
  template <std::ranges::range Range>
  sentinel_for(Range&& range) : value(std::ranges::end(range)) {};
};
template <std::ranges::range Range>
sentinel_for(Range&& range) -> sentinel_for<std::ranges::sentinel_t<Range>,
                                            std::ranges::iterator_t<Range>>;

TRAIT_TEMPLATE_EX(
    ((ValueType), (Reference)), sentinel, dynamic_copyable, (),
    (FN_IMPL(public, bool, equal_to_iterator,
             ((any<anyxx::cref, input_iterator, ValueType, Reference> const&)),
             const,
             [&x](auto const it) {
               return x.value == *unerase_cast<typename T::iterator_type>(it);
             })),
    , , , (using default_proxy_t = nullable_val<iterator_val_proxy_size>;))

template <template <typename...> typename SentinelTrait,
          template <typename...> typename IteratorTrait, typename... Args>
concept is_sentinel_trait_for =
    is_any_derived_from<any_<SentinelTrait, Args...>,
                        any_<sentinel, Args...>> &&
    requires(any_<SentinelTrait, Args...> const& s,
             any_<IteratorTrait, Args...> const& it) {
      { s.equal_to_iterator(it) } -> std::convertible_to<bool>;
    };
template <template <typename...> typename SentinelTrait,
          template <typename...> typename IteratorTrait, typename... Args>
  requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator==(any_<IteratorTrait, Args...> const& it,
                any_<SentinelTrait, Args...> const& s) {
  return s.equal_to_iterator(it);
}
template <template <typename...> typename SentinelTrait,
          template <typename...> typename IteratorTrait, typename... Args>
  requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator!=(any_<IteratorTrait, Args...> const& it,
                any_<SentinelTrait, Args...> const& s) {
  return !s.equal_to_iterator(it);
}
template <template <typename...> typename SentinelTrait,
          template <typename...> typename IteratorTrait, typename... Args>
  requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator==(any_<SentinelTrait, Args...> const& s,
                any_<IteratorTrait, Args...> const& it) {
  return s.equal_to_iterator(it);
}
template <template <typename...> typename SentinelTrait,
          template <typename...> typename IteratorTrait, typename... Args>
  requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator!=(any_<SentinelTrait, Args...> const& s,
                any_<IteratorTrait, Args...> const& it) {
  return !s.equal_to_iterator(it);
}

TRAIT_TEMPLATE_EX(
    ((ValueType), (Reference)), sized_sentinel, sentinel,
    ((ValueType), (Reference)),
    (FN_IMPL(public, std::ptrdiff_t, subtract,
             ((any<anyxx::cref, bidirectional_iterator, ValueType,
                   Reference> const&)),
             const,
             [&x](auto const it) {
               return x.value - *unerase_cast<typename T::iterator_type>(it);
             }),
     FN_IMPL(public, std::ptrdiff_t, subtract_from,
             ((any<anyxx::cref, bidirectional_iterator, ValueType,
                   Reference> const&)),
             const,
             [&x](auto const it) {
               return *unerase_cast<typename T::iterator_type>(it) - x.value;
             })),
    , , , (using default_proxy_t = nullable_val<iterator_val_proxy_size>;))

template <typename Value, typename Ref,
          template <typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any_<sized_sentinel, Value, Ref> const& s,
                         any_<IteratorTrait, Value, Ref> const& it) {
  return s.subtract(it);
}
template <typename Value, typename Ref,
          template <typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any_<IteratorTrait, Value, Ref> const& it,
                         any_<sized_sentinel, Value, Ref> const& s) {
  return s.subtract_from(it);
}

}  // namespace anyxx