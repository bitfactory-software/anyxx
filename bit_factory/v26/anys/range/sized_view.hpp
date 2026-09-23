#pragma once

#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/range/iterators.hpp>
#include <bit_factory/v26/anys/range/iterator_categories.hpp>
#include <bit_factory/v26/anys/range/sentinels.hpp>

namespace anyxx26::rangesxx {

template <is_trait Trait, typename Self, typename Category, typename Value>
struct sized_view : save_copyable<Trait, Self> {
    using default_proxy_t = anyxx::val<std::true_type>;
    template <typename Any>
    using trait_facade_decorator = std::ranges::view_interface<Any>;

    static any<Category::template iterator, Value> begin(Self& self) {
        return std::ranges::begin(self);
    }
    static any<sized_sentinel, Value> end(Self& self) {
        return make_end_sentinel_for(self);
    }
    static any<Category::template iterator, Value const> begin(Self const& self) {
        return std::ranges::begin(self);
    }
    static any<sized_sentinel, Value const> end(Self const& self) {
        return make_end_sentinel_for(self);
    }
    static std::ptrdiff_t size(Self const& self) {
        return std::ranges::size(self);    
    }
    static std::ptrdiff_t reserve_hint(Self const& self) {
#ifdef __cpp_lib_ranges_reserve_hint
        return std::ranges::reserve_hint(self);
#else
        return sized_view::size(self);
#endif
    }
};

}