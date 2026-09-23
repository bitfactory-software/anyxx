#pragma once

#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/range/iterators.hpp>
#include <ranges>

namespace anyxx26::rangesxx {

template <is_trait Trait, typename Self, typename Value>
struct sentinel : save_copyable<Trait, Self> {
    using default_proxy_t = anyxx::val<std::true_type>;

    bool equal(any<input_iterator, anyxx::cref, Value> const&) const;
};
template <template <is_trait, typename, typename...> typename SentinelTrait, template <is_trait, typename, typename...> typename IteratorTrait, typename Value>
concept is_sentinel_trait_for = requires(any<SentinelTrait, Value> const& s, any<IteratorTrait, Value> const& it) {
  { s.equal(it) } -> std::convertible_to<bool>;
};
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename Value>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value>)
bool operator==(any<IteratorTrait, Value> const& it, any<SentinelTrait, Value> const& s) {
    return s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename Value>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value>)
bool operator!=(any<IteratorTrait, Value> const& it, any<SentinelTrait, Value> const& s) {
    return !s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename Value>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value>)
bool operator==(any<SentinelTrait, Value> const& s, any<IteratorTrait, Value> const& it) {
    return s.equal(it);
}
template <template <is_trait, typename, typename...> typename SentinelTrait, template<is_trait, typename, typename...> typename IteratorTrait, typename Value>
    requires(is_sentinel_trait_for<SentinelTrait, IteratorTrait, Value>)
bool operator!=(any<SentinelTrait, Value> const& s, any<IteratorTrait, Value> const& it) {
    return !s.equal(it);
}
template <typename Iterator, typename Sentinel>
struct sentinel_for{
    using iterator_type = Iterator;
    using sentinel_type = Sentinel;
    sentinel_type value;
};
template <std::ranges::range Range>
sentinel_for<std::ranges::iterator_t<Range>, std::ranges::sentinel_t<Range>>
make_end_sentinel_for(Range&& range) {
    return {std::ranges::end(range)};
}
template <typename SentinelFor>
concept is_sentinel_for = requires(SentinelFor s) {
  typename SentinelFor::iterator_type;
  typename SentinelFor::sentinel_type;
  { s.value  };
};
template <is_sentinel_for Sentinel, typename Value>
struct sentinel<model_map, Sentinel, Value> {
    static bool equal(Sentinel const& sentinel, any<input_iterator, anyxx::cref, Value> const& iterator){
        return sentinel.value == *unerase_cast<typename Sentinel::iterator_type>(iterator);
    }
};

template <is_trait Trait, typename Self, typename Value>
struct sized_sentinel : sentinel<Trait, Self, Value> {
    std::ptrdiff_t subtract(any<bidirectional_iterator, anyxx::cref, Value> const&) const;
    std::ptrdiff_t subtract_from(any<bidirectional_iterator, anyxx::cref, Value> const&) const;
};
template <typename Value, template<is_trait, typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any<sized_sentinel, Value> const& s, any<IteratorTrait, Value> const& it) {
    return s.subtract(it);
}
template <typename Value, template<is_trait, typename, typename...> typename IteratorTrait>
std::ptrdiff_t operator-(any<IteratorTrait, Value> const& it, any<sized_sentinel, Value> const& s) {
    return s.subtract_from(it);
}
template <is_sentinel_for Sentinel, typename Value>
struct sized_sentinel<model_map, Sentinel, Value> {
    static std::ptrdiff_t subtract(Sentinel const& sentinel, any<bidirectional_iterator, anyxx::cref, Value> const& iterator){
        return sentinel.value - *unerase_cast<typename Sentinel::iterator_type>(iterator);
    }
    static std::ptrdiff_t subtract_from(Sentinel const& sentinel, any<bidirectional_iterator, anyxx::cref, Value> const& iterator){
        return *unerase_cast<typename Sentinel::iterator_type>(iterator) - sentinel.value;
    }
};
static_assert(std::sized_sentinel_for<any<sized_sentinel, int>,any<bidirectional_iterator, int>>); 

}