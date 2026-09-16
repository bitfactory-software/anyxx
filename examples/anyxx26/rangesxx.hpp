#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx26::rangesxx {

template <typename Self, typename Trait, typename Value>
struct input_iterator : save_copyable<Self, Trait> {
    Value& operator*();
    anyxx::self& operator++();
    bool operator==(anyxx::self const&) const;
    bool operator!=(anyxx::self const&) const;

	struct typenames {
		using value_type = Value;
		using reference = Value&;
		using pointer = Value*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;
	};
};

template <typename Self, typename Trait, typename Value>
struct forward_iterator : input_iterator<Self, Trait, Value> {
	struct typenames : input_iterator<Self, Trait, Value>::typenames {
		using iterator_category = std::forward_iterator_tag;
	};
};

template <typename Self, typename Trait, typename Value>
struct bidirectional_iterator : forward_iterator<Self, Trait, Value> {
    anyxx::self& operator--();

    struct typenames : forward_iterator<Self, Trait, Value>::typenames {
        using iterator_category = std::bidirectional_iterator_tag;
    };
};

}

