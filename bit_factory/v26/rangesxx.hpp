#pragma once

#include <bit_factory/anyxx.hpp>

namespace anyxx26::rangesxx {


template <typename Self, typename Trait, typename Value>
struct input_iterator : save_copyable<Self, Trait> {
    using default_proxy_t = anyxx::val<std::true_type>;

    bool operator==(Self const&) const;
    bool operator!=(Self const&) const;
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

template <typename Self, typename Trait, typename Value>
struct sentinel : save_copyable<Self, Trait> {
    using default_proxy_t = anyxx::val<std::true_type>;

    bool operator==(dyn<input_iterator, Value> const&) const;
    bool operator!=(dyn<input_iterator, Value> const&) const;
};
template <typename Value>
bool operator==(dyn<input_iterator, Value> const& it, dyn<sentinel, Value> const& s) {
    return s == it;
}
template <typename Value>
bool operator!=(dyn<input_iterator, Value> const& it, dyn<sentinel, Value> const& s) {
    return s != it;
}
template <typename Iterator, typename Sentinel>
struct sentinel_for{
    using iterator_type = Iterator;
    using sentinel_type = Sentinel;
    sentinel_type value;
};
template <std::ranges::range Range>
sentinel_for<std::ranges::iterator_t<Range>, std::ranges::sentinel_t<Range>>
make_end_sentinel_for(Range&& range) {
    return {std::ranges::end(range)};
}
template <typename SentinelFor>
concept is_sentinel_for = requires(SentinelFor s) {
  typename SentinelFor::iterator_type;
  typename SentinelFor::sentinel_type;
  { s.value  };
};

template <is_sentinel_for Sentinel, typename Value>
struct sentinel<Sentinel, model_map, Value> {
    static bool op_equals_equals(Sentinel const& sentinel, dyn<input_iterator, Value> const& iterator){
        return sentinel.value == *unerase_cast<typename Sentinel::iterator_type>(iterator);
    }
    static bool op_exclamation_equals(Sentinel const& sentinel, dyn<input_iterator, Value> const& iterator){
        return sentinel.value != *unerase_cast<typename Sentinel::iterator_type>(iterator);
    }
};

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

template <typename Self, typename Trait, typename Category, typename Value>
struct view : save_copyable<Self, Trait> {
    using default_proxy_t = anyxx::val<std::true_type>;
    static dyn<Category::template iterator, Value> begin(Self& self) {
        return std::ranges::begin(self);
    }
    static dyn<sentinel, Value> end(Self& self) {
        return make_end_sentinel_for(self);
    }
    //static dyn<Category::template iterator, Value const> begin(Self const& self) {
    //    return std::ranges::begin(self);
    //}
    //static dyn<sentinel, Value const> end(Self const& self) {
    //    return make_end_sentinel_for(self);
    //}
};

}