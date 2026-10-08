#pragma once

#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/range/iterators.hpp>
#include <ranges>

namespace anyxx26 {

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
    using default_proxy_t = anyxx::val<std::true_type>;

    static bool equal(Self const& self, any<input_iterator, anyxx::cref, Value, Ref> const& it){
        return self.value == *unerase_cast<typename Self::iterator_type>(it);
    }
};
template <template <is_trait, typename, typename...> typename SentinelTrait, template <is_trait, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
concept is_sentinel_trait_for = requires(any<SentinelTrait, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
  { s.equal(it) } -> std::convertible_to<bool>;
};
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value, Ref>)
bool operator==(any<IteratorTrait, Value, Ref> const& it, any<SentinelTrait, Value, Ref> const& s) {
    return s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value, Ref>)
bool operator!=(any<IteratorTrait, Value, Ref> const& it, any<SentinelTrait, Value, Ref> const& s) {
    return !s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value, Ref>)
bool operator==(any<SentinelTrait, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
    return s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename Value, typename Ref>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value, Ref>)
bool operator!=(any<SentinelTrait, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
    return !s.equal(it);
}

template <is_trait Trait, typename Self, typename Value, typename Ref>
struct sized_sentinel : sentinel<Trait, Self, Value, Ref> {
    static std::ptrdiff_t subtract(Self const& self, any<bidirectional_iterator, anyxx::cref, Value, Ref> const& it){
        return self.value - *unerase_cast<typename Self::iterator_type>(it);
    }
    static std::ptrdiff_t subtract_from(Self const& self, any<bidirectional_iterator, anyxx::cref, Value, Ref> const& it){
        return *unerase_cast<typename Self::iterator_type>(it) - self.value;
    }
};
template <typename Value, typename Ref, template<is_trait, typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any<sized_sentinel, Value, Ref> const& s, any<IteratorTrait, Value, Ref> const& it) {
    return s.subtract(it);
}
template <typename Value, typename Ref, template<is_trait, typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any<IteratorTrait, Value, Ref> const& it, any<sized_sentinel, Value, Ref> const& s) {
    return s.subtract_from(it);
}
static_assert(std::sized_sentinel_for<any<sized_sentinel, int, int&>,any<bidirectional_iterator, int, int&>>); 

}