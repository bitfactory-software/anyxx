#pragma once

#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/range/iterators.hpp>
#include <ranges>

namespace anyxx26 {

template <is_trait Trait, typename Self, typename Value, typename Ref = Value&>
struct sentinel : save_copyable<Trait, Self> {
    using default_proxy_t = anyxx::val<std::true_type>;

    bool equal(any<input_iterator, anyxx::cref, Value, Ref> const&) const;
};
template <template <typename, typename, typename...> typename SentinelTrait, template <typename, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
concept is_sentinel_trait_for = requires(any<SentinelTrait, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
  { s.equal(it) } -> std::convertible_to<bool>;
};
template <template <typename, typename, typename...> typename SentinelTrait, template<typename, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value, Ref>)
bool operator==(any<IteratorTrait, Value, Ref> const& it, any<SentinelTrait, Value, Ref> const& s) {
    return s.equal(it);
}
template <template <typename, typename, typename...> typename SentinelTrait, template<typename, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value, Ref>)
bool operator!=(any<IteratorTrait, Value, Ref> const& it, any<SentinelTrait, Value, Ref> const& s) {
    return !s.equal(it);
}
template <template <typename, typename, typename...> typename SentinelTrait, template<typename, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value, Ref>)
bool operator==(any<SentinelTrait, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
    return s.equal(it);
}
template <template <typename, typename, typename...> typename SentinelTrait, template<typename, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value, Ref>)
bool operator!=(any<SentinelTrait, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
    return !s.equal(it);
}
template <typename Sentinel, typename Iterator>
struct sentinel_for{
    using sentinel_type = Sentinel;
    using iterator_type = Iterator;
    sentinel_type value;
    template <std::ranges::range Range>
    sentinel_for(Range&& range) : value(std::ranges::end(range)) {};
};
template <std::ranges::range Range>
sentinel_for(Range&& range) -> sentinel_for<std::ranges::sentinel_t<Range>, std::ranges::iterator_t<Range>>;

template <typename Sentinel, typename Iterator, typename Value, typename Ref>
struct sentinel<model_map, sentinel_for<Sentinel, Iterator>, Value, Ref> {
    static bool equal(sentinel_for<Sentinel, Iterator> const& sentinel, any<input_iterator, anyxx::cref, Value, Ref> const& iterator){
        return sentinel.value == *unerase_cast<Iterator>(iterator);
    }
};

template <is_trait Trait, typename Self, typename Value, typename Ref>
struct sized_sentinel : sentinel<Trait, Self, Value, Ref> {
    std::ptrdiff_t subtract(any<bidirectional_iterator, anyxx::cref, Value, Ref> const&) const;
    std::ptrdiff_t subtract_from(any<bidirectional_iterator, anyxx::cref, Value, Ref> const&) const;
};
template <typename Value, typename Ref, template<typename, typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any<sized_sentinel, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
    return s.subtract(it);
}
template <typename Value, typename Ref, template<typename, typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any<IteratorTrait, Value, Ref> const& it, any<sized_sentinel, Value, Ref> const& s) {
    return s.subtract_from(it);
}

template <typename Sentinel, typename Iterator, typename Value, typename Ref>
struct sized_sentinel<model_map, sentinel_for<Sentinel, Iterator>, Value, Ref> {
    static std::ptrdiff_t subtract(sentinel_for<Sentinel, Iterator> const& sentinel, any<bidirectional_iterator, anyxx::cref, Value, Ref> const& iterator){
        return sentinel.value - *unerase_cast<Iterator>(iterator);
    }
    static std::ptrdiff_t subtract_from(sentinel_for<Sentinel, Iterator> const& sentinel, any<bidirectional_iterator, anyxx::cref, Value, Ref> const& iterator){
        return *unerase_cast<Iterator>(iterator) - sentinel.value;
    }
};
static_assert(std::sized_sentinel_for<any<sized_sentinel, int, int&>,any<bidirectional_iterator, int, int&>>); 

}