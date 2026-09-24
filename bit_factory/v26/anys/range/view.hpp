#pragma once

#include <bit_factory/v26/anys/range/view_base.hpp>
#include <bit_factory/v26/anys/range/sentinels.hpp>

namespace anyxx26 {

template <is_trait Trait, typename Self, typename Category, typename Value>
struct view : view_base<Trait, Self, Category, Value> {
    static any<sentinel, Value> end(Self& self) {
        return sentinel_for(self);
    }
    static any<sentinel, Value const> end(Self const& self) {
        return sentinel_for(self);
    }
};

}