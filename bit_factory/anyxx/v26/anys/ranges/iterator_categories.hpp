#pragma once

#include <bit_factory/anyxx/v26/any.hpp>
#include <bit_factory/anyxx/v26/anys/ranges/iterators.hpp>

namespace anyxx {

struct input {
    template <typename Self, typename Value, typename Ref = Value&>
    using iterator = input_iterator<Self, Value, Ref>;
};
struct forward {
    template <typename Self, typename Value, typename Ref = Value&>
    using iterator = forward_iterator<Self, Value, Ref>;
};
struct bidirectional {
    template <typename Self, typename Value, typename Ref = Value&>
    using iterator = bidirectional_iterator<Self, Value, Ref>;
};
struct  random_access {
    template <typename Self, typename Value, typename Ref = Value&>
    using iterator = random_access_iterator<Self, Value, Ref>;
};
struct  contiguous {
    template <typename Self, typename Value, typename Ref = Value&>
    using iterator = contiguous_iterator<Self, Value, Ref>;
};

}