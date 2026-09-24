#pragma once

#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/range/iterators.hpp>
#include <bit_factory/v26/anys/range/iterator_categories.hpp>

namespace anyxx26 {

template <is_trait Trait, typename Self, typename Category, typename Value, typename Ref = Value&>
struct view_base : save_copyable<Trait, Self> {
    using default_proxy_t = anyxx::val<std::true_type>;
    template <typename Any>
    using trait_facade_decorator = std::ranges::view_interface<Any>;
    template <typename ConstructedWith>
    static auto preprocess_constructed_with(ConstructedWith&& constructed_with) {
      if constexpr (std::ranges::view<ConstructedWith>) {
        return std::forward<ConstructedWith>(constructed_with);
      } else {
        return std::views::all(std::forward<ConstructedWith>(constructed_with));
      }
    }

    static any<Category::template iterator, Value, Ref> begin(Self& self) {
        static_assert(std::ranges::view<Self>, "Self must be a view");
        return std::ranges::begin(self);
    }
    static any<Category::template iterator, std::add_const_t<Value>, std::add_const_t<Ref>> begin(Self const& self) {
        return std::ranges::begin(self);
    }
};

}