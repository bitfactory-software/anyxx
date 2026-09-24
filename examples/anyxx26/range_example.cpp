#include <array>
#include <bit_factory/v26/anys/range.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ranges>

using namespace anyxx26;

static_assert(has_deduced_typenames<input_iterator, int>);
static_assert(has_identifier(compute_deduced_typenames<input_iterator, int>()));
static_assert(has_identifier(^^ input_iterator<declaration, declaration, int>::typenames));
static_assert(std::same_as<deduced_typenames<input_iterator, int>, input_iterator<declaration, declaration, int>::typenames>);
static_assert(std::input_iterator<any<input_iterator, int>>);
static_assert(std::forward_iterator<any<forward_iterator, int>>);
static_assert(std::bidirectional_iterator<any<bidirectional_iterator, int>>);
static_assert(std::random_access_iterator<any<random_access_iterator, int>>);
static_assert(std::contiguous_iterator<any<contiguous_iterator, int>>);

static_assert(std::sentinel_for<any<sentinel, int>, any<input_iterator, int>>);
static_assert(std::sentinel_for<any<sentinel, int>, any<forward_iterator, int>>);
static_assert(std::sentinel_for<any<sentinel, int>, any<bidirectional_iterator, int>>);
static_assert(std::sentinel_for<any<sentinel, int>, any<random_access_iterator, int>>);
static_assert(std::sentinel_for<any<sentinel, int>, any<contiguous_iterator, int>>);

namespace {

void test_input_iterator(any<input_iterator, int> begin, any<sentinel, int> end) {
    auto expected = 1;
    std::ranges::for_each(begin, end, [&expected](int x){ CHECK(x == expected++); });
}
void test_input_iterator(any<input_iterator, int> begin, any<input_iterator, int> end) {
    auto expected = 1;
    std::ranges::for_each(begin, end, [&expected](int x){ CHECK(x == expected++); });
}
void test_input_range(any<view, input, int> const& r) {
    auto expected = 1;
    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
}
//void test_const_input_range(any<view, input, int const> const& r) {
//    auto expected = 1;
//    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
//}
void test_input_range_template(const std::ranges::range auto&& r) {
    auto expected = 1;
    for (auto const x : std::forward<decltype(r)>(r)) {
        CHECK(x == expected++);
    }
}

void test_forward_range(any<view, forward, int const> r) {
    auto expected = 1;
    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
}

void test_bidirectional_iterator(any<bidirectional_iterator, int> begin) {
    CHECK(*begin++ == 1);
    CHECK(*--begin == 1);
    CHECK(*begin == 1);
    CHECK(*++begin == 2);
    CHECK(*begin == 2);
    CHECK(*--begin == 1);
    CHECK(*begin == 1);
}

void test_random_access_iterator(any<random_access_iterator, int> begin, any<random_access_iterator, int> end) {
    CHECK(begin < end);
    CHECK(!(begin > end));
    CHECK(begin <= end);
    CHECK(!(begin >= end));
    CHECK(5u == (end - begin));
    begin += 1u;
    CHECK(*begin == 2);
    begin -= 1u;
    CHECK(*begin == 1);
    CHECK(begin[2] == 3);
}

struct a_struct {
    int i = 0;
};

void test_contiguous_iterator(any<contiguous_iterator, a_struct> begin) {
    CHECK(begin->i == 1);
}
}

TEST_CASE("anyxx26 iterators ranges") {

	std::array<int, 5> arr{ 1, 2, 3, 4, 5 };
    std::array<int, 5> const const_arr{ 1, 2, 3, 4, 5 };

    auto e = sentinel_for(arr);
    any<sentinel, int> end_sentinel{e};
	test_input_iterator(arr.begin(), end_sentinel);
    
    any<input_iterator, int> end_iterator{arr.end()};
    test_input_iterator(arr.begin(), end_iterator);
    
    test_input_range(arr);
    test_input_range(const_arr);
    //test_const_input_range(std::views::iota(1, 6));
    test_input_range_template(any<view, input, int>{const_arr});

    test_forward_range(arr);
    test_forward_range(const_arr);

    test_bidirectional_iterator(arr.begin());
    
    test_random_access_iterator(arr.begin(), arr.end());

    std::array<a_struct, 2> arr2{ 1, 2 };
    test_contiguous_iterator(arr2.begin());
    any<view, contiguous, a_struct> cr{arr2};
    static_assert(std::ranges::view<any<view, contiguous, a_struct>>);

    any<sized_view, contiguous, a_struct> sr{ arr2 };
    static_assert(std::ranges::sized_range<any<sized_view, contiguous, a_struct>>);
    static_assert(has_trait_facade_decorator<sized_view, contiguous, a_struct>);
    CHECK(sr.front().i == 1);
    CHECK(!sr.empty());
    CHECK(sr);
    CHECK(sr.size() == 2);
    CHECK(sr[0].i == 1);
    sr[0].i = 42;
    CHECK(sr[0].i == 42);
}
