#pragma once

#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/range/iterators.hpp>

namespace anyxx26 {

struct input {
    template <typename Self, typename Trait, typename Value, typename Ref = Value&>
    using iterator = input_iterator<Trait, Self, Value, Ref>;
};
struct forward {
    template <typename Self, typename Trait, typename Value, typename Ref = Value&>
    using iterator = forward_iterator<Trait, Self, Value, Ref>;
};
struct bidirectional {
    template <typename Self, typename Trait, typename Value, typename Ref = Value&>
    using iterator = bidirectional_iterator<Trait, Self, Value, Ref>;
};
struct  random_access {
    template <typename Self, typename Trait, typename Value, typename Ref = Value&>
    using iterator = random_access_iterator<Trait, Self, Value, Ref>;
};
struct  contiguous {
    template <typename Self, typename Trait, typename Value, typename Ref = Value&>
    using iterator = contiguous_iterator<Trait, Self, Value, Ref>;
};

}