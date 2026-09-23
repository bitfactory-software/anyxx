#pragma once

#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/range/iterators.hpp>

namespace anyxx26 {

struct input {
    template <typename Self, typename Trait, typename Value>
    using iterator = input_iterator<Self, Trait, Value>;
};
struct forward {
    template <typename Self, typename Trait, typename Value>
    using iterator = forward_iterator<Self, Trait, Value>;
};
struct bidirectional {
    template <typename Self, typename Trait, typename Value>
    using iterator = bidirectional_iterator<Self, Trait, Value>;
};
struct  random_access {
    template <typename Self, typename Trait, typename Value>
    using iterator = random_access_iterator<Self, Trait, Value>;
};
struct  contiguous {
    template <typename Self, typename Trait, typename Value>
    using iterator = contiguous_iterator<Self, Trait, Value>;
};

}