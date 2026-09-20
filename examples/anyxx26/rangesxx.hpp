#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx26::rangesxx {

template <typename Self, typename Trait>
struct sentinel : save_copyable<Self, Trait> {
    using default_proxy_t = anyxx::val<std::true_type>;

    bool operator==(Self const&) const;
    bool operator!=(Self const&) const;
};

template <typename Self, typename Trait, typename Value>
struct input_iterator : sentinel<Self, Trait> {


    Value& operator*();
    Self& operator++();

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
    Self& operator--();

    struct typenames : forward_iterator<Self, Trait, Value>::typenames {
        using iterator_category = std::bidirectional_iterator_tag;
    };
};

template <typename Self, typename Trait, typename Value>
struct random_access_iterator : bidirectional_iterator<Self, Trait, Value> {
    bool operator<(Self const&) const;
    bool operator>(Self const&) const;
    bool operator<=(Self const&) const;
    bool operator>=(Self const&) const;
    Self& operator-=(std::ptrdiff_t);
    Self& operator+=(std::ptrdiff_t);
    Self operator-(std::ptrdiff_t) const;
    Self operator+(std::ptrdiff_t) const;
    std::ptrdiff_t operator-(Self const&) const;
    Value& operator[](std::ptrdiff_t) const;

    struct typenames : bidirectional_iterator<Self, Trait, Value>::typenames {
        using iterator_category = std::random_access_iterator_tag;
    };
};
template <template<typename, typename, typename...> typename IteratorTrait, typename Value>
auto operator+(std::ptrdiff_t n, dyn<IteratorTrait, Value> const& it) {
    return it + n;
}

template <typename Self, typename Trait, typename Value>
struct contiguous_iterator : random_access_iterator<Self, Trait, Value> {
    Value* operator->() const;

    struct typenames : random_access_iterator<Self, Trait, Value>::typenames {
        using iterator_category = std::contiguous_iterator_tag;
    };
};

}

