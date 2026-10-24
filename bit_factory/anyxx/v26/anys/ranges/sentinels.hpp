#pragma once

#include <bit_factory/anyxx/v26/any.hpp>
#include <bit_factory/anyxx/v26/anys/ranges/iterators.hpp>
#include <ranges>

namespace anyxx {

template <typename Sentinel, typename Iterator>
struct sentinel_for{
    using iterator_type = Iterator;
    Sentinel value;
    template <std::ranges::range Range>
    sentinel_for(Range&& range) : value(std::ranges::end(range)) {};
};
template <std::ranges::range Range>
sentinel_for(Range&& range) -> sentinel_for<std::ranges::sentinel_t<Range>, std::ranges::iterator_t<Range>>;

template <typename Self, typename Value, typename Ref = Value&>
struct sentinel : copyable<Self> {
    using default_proxy_t = anyxx::nullable_val<>;

    static bool equal(Self const& self, any_<anyxx::cref, input_iterator, Value, Ref> const& it){
        return self.value == any_cast<typename Self::iterator_type>(it);
    }
};
template <template <typename, typename...> typename SentinelTrait, template <typename, typename...> typename IteratorTrait, typename... Args>
concept is_sentinel_trait_for = requires(any<SentinelTrait, Args...> const& s, any<IteratorTrait, Args...> const& it) {
  { s.equal(it) } -> std::convertible_to<bool>;
};
template <template <typename, typename...> typename SentinelTrait, template<typename, typename...> typename IteratorTrait, typename... Args>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator==(any<IteratorTrait, Args...> const& it, any<SentinelTrait, Args...> const& s) {
    return s.equal(it);
}
template <template <typename, typename...> typename SentinelTrait, template<typename, typename...> typename IteratorTrait, typename... Args>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator!=(any<IteratorTrait, Args...> const& it, any<SentinelTrait, Args...> const& s) {
    return !s.equal(it);
}
template <template <typename, typename...> typename SentinelTrait, template<typename, typename...> typename IteratorTrait, typename... Args>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator==(any<SentinelTrait, Args...> const& s, any<IteratorTrait, Args...> const& it) {
    return s.equal(it);
}
template <template <typename, typename...> typename SentinelTrait, template<typename, typename...> typename IteratorTrait, typename... Args>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator!=(any<SentinelTrait, Args...> const& s, any<IteratorTrait, Args...> const& it) {
    return !s.equal(it);
}
static_assert(is_sentinel_trait_for<sentinel, input_iterator, int>);
static_assert(is_sentinel_trait_for<sentinel, input_iterator, int, int const&>);
static_assert(is_sentinel_trait_for<sentinel, random_access_iterator, int>);
static_assert(is_sentinel_trait_for<sentinel, random_access_iterator, int, int const&>);
static_assert(std::sentinel_for<any<sentinel, int>, any<input_iterator, int>>);
static_assert(std::sentinel_for<any<sentinel, int, const int&>, any<input_iterator, int, const int&>>);


template <typename Self, typename Value, typename Ref>
struct sized_sentinel : sentinel<Self, Value, Ref> {
    static std::ptrdiff_t subtract(Self const& self, any_<anyxx::cref, bidirectional_iterator, Value, Ref> const& it){
        return self.value - any_cast<typename Self::iterator_type>(it);
    }
    static std::ptrdiff_t subtract_from(Self const& self, any_<anyxx::cref, bidirectional_iterator, Value, Ref> const& it){
        return any_cast<typename Self::iterator_type>(it) - self.value;
    }
};
template <typename Value, typename Ref, template<typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any<sized_sentinel, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
    return s.subtract(it);
}
template <typename Value, typename Ref, template<typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any<IteratorTrait, Value, Ref> const& it, any<sized_sentinel, Value, Ref> const& s) {
    return s.subtract_from(it);
}

static_assert(std::sized_sentinel_for<any<sized_sentinel, int, int&>,any<bidirectional_iterator, int, int&>>); 
static_assert(std::sentinel_for<any<sentinel, int, int&>, any<input_iterator, int, int&>>);

}