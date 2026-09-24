#pragma once

#include <bit_factory/v26/any.hpp>
#include <ranges>

namespace anyxx26 {

template <is_trait Trait, typename Self, typename Value, typename Ref = Value&>
struct input_iterator : save_copyable<Trait, Self> {
    using default_proxy_t = anyxx::val<std::true_type>;

    bool operator==(Self const&) const;
    bool operator!=(Self const&) const;
    Self& operator++();

    //Ref operator*() const;
    static Ref op_star(Self const& self) { 
        if constexpr (std::is_reference_v<std::remove_const_t<Ref>>) {
            return const_cast<Ref>(*self);
        } else {
            return *self;
        }
    }

	struct typenames {
		using value_type = Value;
		using reference = Ref;
		using pointer = Value*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;
	};
};

template <is_trait Trait, typename Self, typename Value, typename Ref = Value&>
struct forward_iterator : input_iterator<Trait, Self, Value, Ref> {
	struct typenames : input_iterator<Trait, Self, Value, Ref>::typenames {
		using iterator_category = std::forward_iterator_tag;
	};
};

template <is_trait Trait, typename Self, typename Value, typename Ref = Value&>
struct bidirectional_iterator : forward_iterator<Trait, Self, Value, Ref> {
    Self& operator--();

    struct typenames : forward_iterator<Trait, Self, Value, Ref>::typenames {
        using iterator_category = std::bidirectional_iterator_tag;
    };
};

template <is_trait Trait, typename Self, typename Value, typename Ref = Value&>
struct random_access_iterator : bidirectional_iterator<Trait, Self, Value, Ref> {
    bool operator<(Self const&) const;
    bool operator>(Self const&) const;
    bool operator<=(Self const&) const;
    bool operator>=(Self const&) const;
    Self& operator-=(std::ptrdiff_t);
    Self& operator+=(std::ptrdiff_t);
    Self operator-(std::ptrdiff_t) const;
    Self operator+(std::ptrdiff_t) const;
    std::ptrdiff_t operator-(Self const&) const;
    //Ref const& operator[](std::ptrdiff_t) const;
    static Ref op_square_brackets(Self const& self, std::ptrdiff_t offset) {
        if constexpr(std::is_reference_v<std::remove_const_t<Ref>>) {
            return const_cast<Ref>(self[offset]);
        } else {    
            return self[offset];
        }
    }

    struct typenames : bidirectional_iterator<Trait, Self, Value, Ref>::typenames {
        using iterator_category = std::random_access_iterator_tag;
    };
};
template <template<is_trait, typename, typename...> typename IteratorTrait, typename Value, typename Ref = Value&>
auto operator+(std::ptrdiff_t n, any<IteratorTrait, Value, Ref> const& it) {
    return it + n;
}

template <is_trait Trait, typename Self, typename Value, typename Ref = Value&>
struct contiguous_iterator : random_access_iterator<Trait, Self, Value, Ref> {
    //Value* operator->() const;
    static Value* op_arrow(Self const& self){
         return const_cast<Value*>(&(*self)); 
    }

    struct typenames : random_access_iterator<Trait, Self, Value, Ref>::typenames {
        using iterator_category = std::contiguous_iterator_tag;
    };
};

}