#pragma once

#include <bit_factory/anyxx/anys/ranges/iterators.hpp>

namespace anyxx {

struct input {
    template <typename Value, typename Ref = Value&>
    using iterator = input_iterator<Value, Ref>;
};
struct forward {
    template <typename Value, typename Ref = Value&>
    using iterator = forward_iterator<Value, Ref>;
};
struct bidirectional {
    template <typename Value, typename Ref = Value&>
    using iterator = bidirectional_iterator<Value, Ref>;
};
struct random_access {
    template <typename Value, typename Ref = Value&>
    using iterator = random_access_iterator<Value, Ref>;
};
struct contiguous {
    template <typename Value, typename Ref = Value&>
    using iterator = contiguous_iterator<Value, Ref>;
};

}