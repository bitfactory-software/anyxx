#include <array>
#include <bit_factory/v26/anys/ranges.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include <print>

#if defined(__GNUC__) and !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

using namespace anyxx;

TEST_CASE("anyxx iterators sentinel equality") {

	std::array<int, 5> arr{ 1, 2, 3, 4, 5 };

	auto e = sentinel_for(arr);
    any_<sentinel, int const, int const&> end_sentinel{e};
	any_<forward_iterator, int const, int const&> begin_iterator{arr.begin()};
	CHECK(!(begin_iterator == end_sentinel));

    any_<view, forward, int const, int const&> r{std::views::all(arr)};
    for (auto const& x : r) {
        CHECK(x == arr[x - 1]);
    }

}

namespace {

void test_input_iterator(any_<input_iterator, int> begin, any_<sentinel, int> end) {
    auto expected = 1;
    std::ranges::for_each(begin, end, [&expected](int x){ CHECK(x == expected++); });
}
void test_input_iterator(any_<input_iterator, int> begin, any_<input_iterator, int> end) {
    auto expected = 1;
    std::ranges::for_each(begin, end, [&expected](int x){ CHECK(x == expected++); });
}
void test_input_range(any_<view, input, int, int&> r) {
    auto expected = 1;
    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
}
void test_const_input_range(any_<view, input, int const> const& r) {
    auto expected = 1;
    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
}
void test_const_input_range1(any_<view, input, int, int> const& r) {
    auto expected = 1;
    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
}
void test_input_range_template(const std::ranges::range auto&& r) {
    auto expected = 1;
    for (auto const x : std::forward<decltype(r)>(r)) {
        CHECK(x == expected++);
    }
}

void test_forward_range(any_<view, forward, int const>const& r) {
    auto expected = 1;
    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
}

void test_bidirectional_iterator(any_<bidirectional_iterator, int> begin) {
    CHECK(*begin++ == 1);
    CHECK(*--begin == 1);
    CHECK(*begin == 1);
    CHECK(*++begin == 2);
    CHECK(*begin == 2);
    CHECK(*--begin == 1);
    CHECK(*begin == 1);
}

void test_random_access_iterator(any_<random_access_iterator, int> begin, any_<random_access_iterator, int> end) {
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

void test_contiguous_iterator(any_<contiguous_iterator, a_struct> begin) {
    CHECK(begin->i == 1);
}

class container{
public:
   any_<sized_view, contiguous, a_struct const> get(int i) {
       if (i == 0) { return v; }
       else if(i == 1) { return arr; }
       else { return std::views::single(single); }
   }

private:
    std::vector<a_struct> v{ {1}, {2}, {3}, {4}, {5} };
    std::array<a_struct, 5> arr{1, 2, 3, 4, 5};
    a_struct single{1};
};
}

TEST_CASE("anyxx iterators ranges") {

	std::array<int, 5> arr{ 1, 2, 3, 4, 5 };
    std::array<int, 5> const const_arr{ 1, 2, 3, 4, 5 };
    std::array<a_struct, 5> arrs{ 1, 2, 3, 4, 5 };

    auto e = sentinel_for(arr);
    any_<sentinel, int> end_sentinel{e};
	test_input_iterator(arr.begin(), end_sentinel);
    
    any_<input_iterator, int> end_iterator{arr.end()};
    test_input_iterator(arr.begin(), end_iterator);

    test_input_range(any_<view, input, int>{std::in_place, std::views::all(arr)});
    test_const_input_range(const_arr);
    test_const_input_range1(any_<view, input, int, int>{std::in_place_type<std::ranges::iota_view<int, int>>, 1, 6});
    test_const_input_range1(any_<view, input, int, int>{std::ranges::iota_view<int, int>(1, 6)});
    test_const_input_range1(std::ranges::iota_view<int, int>(1, 6));
    test_input_range_template(any_<view, input, int const>{const_arr});

    test_forward_range(const_arr);

    test_bidirectional_iterator(arr.begin());
    
    test_random_access_iterator(arr.begin(), arr.end());

    std::array<a_struct, 2> arr2{ 1, 2 };
    test_contiguous_iterator(arr2.begin());
    any_<view, contiguous, a_struct> cr{arr2};
    CHECK(cr.begin()->i == 1);

    any_<sized_view, contiguous, a_struct> sr{ arr2 };
    CHECK(sr.front().i == 1);
    CHECK(!sr.empty());
    CHECK(sr);
    CHECK(sr.size() == 2);
    CHECK(sr[0].i == 1);
    sr[0].i = 42;
    CHECK(sr[0].i == 42);
    CHECK(arr2[0].i == 42);

    container c;
    auto r1 = c.get(0);
    CHECK(r1.front().i == 1);
    CHECK(!r1.empty());
    CHECK(r1);
    CHECK(r1.size() == 5);
    CHECK(r1[0].i == 1);

    for(auto i : std::views::iota(0, 3)) {
        for(auto const& x : c.get(i)) {
            std::println("   {}", x.i);
        }
        std::println();
    }

}
