#pragma once

#include <bit_factory/v26/any.hpp>
#include <ranges>

namespace anyxx26 {

template <is_trait Trait, typename Self, typename Value>
struct input_iterator : save_copyable<Trait, Self> {
    using default_proxy_t = anyxx::val<std::true_type>;

    bool operator==(Self const&) const;
    bool operator!=(Self const&) const;
    Value& operator*() const;
    Self& operator++();

	struct typenames {
		using value_type = Value;
		using reference = Value&;
		using pointer = Value*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;
	};
};

template <is_trait Trait, typename Self, typename Value>
struct forward_iterator : input_iterator<Trait, Self, Value> {
	struct typenames : input_iterator<Trait, Self, Value>::typenames {
		using iterator_category = std::forward_iterator_tag;
	};
};

template <is_trait Trait, typename Self, typename Value>
struct bidirectional_iterator : forward_iterator<Trait, Self, Value> {
    Self& operator--();

    struct typenames : forward_iterator<Trait, Self, Value>::typenames {
        using iterator_category = std::bidirectional_iterator_tag;
    };
};

template <is_trait Trait, typename Self, typename Value>
struct random_access_iterator : bidirectional_iterator<Trait, Self, Value> {
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

    struct typenames : bidirectional_iterator<Trait, Self, Value>::typenames {
        using iterator_category = std::random_access_iterator_tag;
    };
};
template <template<is_trait, typename, typename...> typename IteratorTrait, typename Value>
auto operator+(std::ptrdiff_t n, any<IteratorTrait, Value> const& it) {
    return it + n;
}

template <is_trait Trait, typename Self, typename Value>
struct contiguous_iterator : random_access_iterator<Trait, Self, Value> {
    Value* operator->() const;

    struct typenames : random_access_iterator<Trait, Self, Value>::typenames {
        using iterator_category = std::contiguous_iterator_tag;
    };
};

}