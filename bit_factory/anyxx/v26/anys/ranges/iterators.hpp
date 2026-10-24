#pragma once

#include <bit_factory/anyxx/v26/any.hpp>
#include <ranges>

namespace anyxx {

template <typename Self, typename Value, typename Ref = Value&>
struct input_iterator : typed_copyable<Self> {
    using default_proxy_t = anyxx::nullable_val<>;

    bool operator==(Self const&) const;
    bool operator!=(Self const&) const;
    Self& operator++();
    Ref operator*() const;

	struct typenames {
		using value_type = Value;
		using reference = Ref;
		using pointer = Value*;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::input_iterator_tag;
	};
};

template <typename Self, typename Value, typename Ref = Value&>
struct forward_iterator : input_iterator<Self, Value, Ref> {
	struct typenames : input_iterator<Self, Value, Ref>::typenames {
		using iterator_category = std::forward_iterator_tag;
	};
};

template <typename Self, typename Value, typename Ref = Value&>
struct bidirectional_iterator : forward_iterator<Self, Value, Ref> {
    Self& operator--();

    struct typenames : forward_iterator<Self, Value, Ref>::typenames {
        using iterator_category = std::bidirectional_iterator_tag;
    };
};

template <typename Self, typename Value, typename Ref = Value&>
struct random_access_iterator : bidirectional_iterator<Self, Value, Ref> {
    bool operator<(Self const&) const;
    bool operator>(Self const&) const;
    bool operator<=(Self const&) const;
    bool operator>=(Self const&) const;
    Self& operator-=(std::ptrdiff_t);
    Self& operator+=(std::ptrdiff_t);
    Self operator-(std::ptrdiff_t) const;
    Self operator+(std::ptrdiff_t) const;
    std::ptrdiff_t operator-(Self const&) const;
    Ref operator[](std::ptrdiff_t) const;
    //static Ref op_square_brackets(Self const& self, std::ptrdiff_t offset) {
    //    if constexpr(std::is_reference_v<std::remove_const_t<Ref>>) {
    //        return const_cast<Ref>(self[offset]);
    //    } else {    
    //        return self[offset];
    //    }
    //}

    struct typenames : bidirectional_iterator<Self, Value, Ref>::typenames {
        using iterator_category = std::random_access_iterator_tag;
    };
};
template <template<typename, typename...> typename IteratorTrait, typename... Args>
auto operator+(std::ptrdiff_t n, any<IteratorTrait, Args...> const& it) {
    return it + n;
}

template <typename Self, typename Value, typename Ref = Value&>
struct contiguous_iterator : random_access_iterator<Self, Value, Ref> {
    Value* operator->() const;

    struct typenames : random_access_iterator<Self, Value, Ref>::typenames {
        using iterator_category = std::contiguous_iterator_tag;
    };
};

}