#pragma once

#include <bit_factory/v26/anys/range/view_base.hpp>
#include <bit_factory/v26/anys/range/sentinels.hpp>

namespace anyxx26 {

template <is_trait Trait, typename Self, typename Category, typename Value, typename Ref = Value&>
struct view : view_base<Trait, Self, Category, Value, Ref> {
    static any<sentinel, Value, Ref> end(Self& self) {
        return sentinel_for(self);
    }
    static any<sentinel, std::add_const_t<Value>, std::add_const_t<Ref>> end(Self const& self) {
        return sentinel_for(self);
    }
};

}