#pragma once

#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/range/iterators.hpp>
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

template <is_trait Trait, typename Self, typename Value, typename Ref = Value&>
struct sentinel : copyable<Trait, Self> {
    using default_proxy_t = anyxx::nullable_val<>;

    static bool equal(Self const& self, any<anyxx::cref, input_iterator, Value, Ref> const& it){
        return self.value == *unerase_cast<typename Self::iterator_type>(it);
    }
};
template <template <is_trait, typename, typename...> typename SentinelTrait, template <is_trait, typename, typename...> typename IteratorTrait, typename... Args>
concept is_sentinel_trait_for = requires(any_<SentinelTrait, Args...> const& s, any_<IteratorTrait, Args...> const& it) {
  { s.equal(it) } -> std::convertible_to<bool>;
};
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename... Args>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator==(any_<IteratorTrait, Args...> const& it, any_<SentinelTrait, Args...> const& s) {
    return s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename... Args>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator!=(any_<IteratorTrait, Args...> const& it, any_<SentinelTrait, Args...> const& s) {
    return !s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename... Args>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator==(any_<SentinelTrait, Args...> const& s, any_<IteratorTrait, Args...> const& it) {
    return s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename... Args>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Args...>)
bool operator!=(any_<SentinelTrait, Args...> const& s, any_<IteratorTrait, Args...> const& it) {
    return !s.equal(it);
}
static_assert(is_sentinel_trait_for<sentinel, input_iterator, int>);
static_assert(is_sentinel_trait_for<sentinel, input_iterator, int, int const&>);
static_assert(is_sentinel_trait_for<sentinel, random_access_iterator, int>);
static_assert(is_sentinel_trait_for<sentinel, random_access_iterator, int, int const&>);
static_assert(std::sentinel_for<any_<sentinel, int>, any_<input_iterator, int>>);
static_assert(std::sentinel_for<any_<sentinel, int, const int&>, any_<input_iterator, int, const int&>>);


template <is_trait Trait, typename Self, typename Value, typename Ref>
struct sized_sentinel : sentinel<Trait, Self, Value, Ref> {
    static std::ptrdiff_t subtract(Self const& self, any<anyxx::cref, bidirectional_iterator, Value, Ref> const& it){
        return self.value - *unerase_cast<typename Self::iterator_type>(it);
    }
    static std::ptrdiff_t subtract_from(Self const& self, any<anyxx::cref, bidirectional_iterator, Value, Ref> const& it){
        return *unerase_cast<typename Self::iterator_type>(it) - self.value;
    }
};
template <typename Value, typename Ref, template<is_trait, typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any_<sized_sentinel, Value, Ref> const& s, any_<IteratorTrait, Value, Ref> const& it) {
    return s.subtract(it);
}
template <typename Value, typename Ref, template<is_trait, typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any_<IteratorTrait, Value, Ref> const& it, any_<sized_sentinel, Value, Ref> const& s) {
    return s.subtract_from(it);
}

static_assert(std::sized_sentinel_for<any_<sized_sentinel, int, int&>,any_<bidirectional_iterator, int, int&>>); 
static_assert(std::sentinel_for<any_<sentinel, int, int&>, any_<input_iterator, int, int&>>);

}