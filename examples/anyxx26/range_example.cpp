#include <array>
#include <bit_factory/v26/any/range.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ranges>

using namespace anyxx26;

static_assert(has_deduced_typenames<rangesxx::input_iterator, int>);
static_assert(has_identifier(compute_deduced_typenames<rangesxx::input_iterator, int>()));
static_assert(has_identifier(^^ rangesxx::input_iterator<declaration, declaration, int>::typenames));
static_assert(std::same_as<deduced_typenames<rangesxx::input_iterator, int>, rangesxx::input_iterator<declaration, declaration, int>::typenames>);
static_assert(std::input_iterator<dyn<rangesxx::input_iterator, int>>);
static_assert(std::sentinel_for<dyn<rangesxx::sentinel, int>, dyn<rangesxx::input_iterator, int>>);
static_assert(std::forward_iterator<dyn<rangesxx::forward_iterator, int>>);
static_assert(std::bidirectional_iterator<dyn<rangesxx::bidirectional_iterator, int>>);
static_assert(std::random_access_iterator<dyn<rangesxx::random_access_iterator, int>>);
static_assert(std::contiguous_iterator<dyn<rangesxx::contiguous_iterator, int>>);

namespace {

void test_input_iterator(dyn<rangesxx::input_iterator, int> begin, dyn<rangesxx::sentinel, int> end) {
    auto expected = 1;
    std::ranges::for_each(begin, end, [&expected](int x){ CHECK(x == expected++); });
}
void test_input_iterator(dyn<rangesxx::input_iterator, int> begin, dyn<rangesxx::input_iterator, int> end) {
    auto expected = 1;
    std::ranges::for_each(begin, end, [&expected](int x){ CHECK(x == expected++); });
}
//void test_input_range(dyn<rangesxx::view, rangesxx::input, int>& r) {
//    auto expected = 1;
//    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
//}
void test_input_range(dyn<rangesxx::view, rangesxx::input, int> const& r) {
    auto expected = 1;
    std::ranges::for_each(r, [&expected](int x){ CHECK(x == expected++); });
}
void test_input_range_template(const std::ranges::range auto&& r) {
    auto expected = 1;
    for (auto const x : std::forward<decltype(r)>(r)) {
        CHECK(x == expected++);
    }
}

void test_bidirectional_iterator(dyn<rangesxx::bidirectional_iterator, int> begin) {
    CHECK(*begin++ == 1);
    CHECK(*--begin == 1);
    CHECK(*begin == 1);
    CHECK(*++begin == 2);
    CHECK(*begin == 2);
    CHECK(*--begin == 1);
    CHECK(*begin == 1);
}

void test_random_access_iterator(dyn<rangesxx::random_access_iterator, int> begin, dyn<rangesxx::random_access_iterator, int> end) {
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

void test_contiguous_iterator(dyn<rangesxx::contiguous_iterator, a_struct> begin) {
    CHECK(begin->i == 1);
}
}

TEST_CASE("anyxx26 iterators ranges") {

	std::array<int, 5> arr{ 1, 2, 3, 4, 5 };
    std::array<int, 5> const const_arr{ 1, 2, 3, 4, 5 };

    auto e = rangesxx::make_end_sentinel_for(arr);
    dyn<rangesxx::sentinel, int> end_sentinel{e};
	test_input_iterator(arr.begin(), end_sentinel);
    
    dyn<rangesxx::input_iterator, int> end_iterator{arr.end()};
    test_input_iterator(arr.begin(), end_iterator);
    
    test_input_range(arr);
    test_input_range(const_arr);
    test_input_range_template(dyn<rangesxx::view, rangesxx::input, int>{const_arr});

    test_bidirectional_iterator(arr.begin());
    
    test_random_access_iterator(arr.begin(), arr.end());

    std::array<a_struct, 2> arr2{ 1, 2 };
    test_contiguous_iterator(arr2.begin());

//    meta::print_members<dyn<rangesxx::random_access_iterator,anyxx::val<std::true_type>, int>>();
//    using v_table_members_t =[:make_v_table_members_type<rangesxx::random_access_iterator, int>():];
//    using v_table_members_t =[:make_v_table_members_type<rangesxx::bidirectional_iterator, int>():];
//    meta::print_members<v_table_members_t>();
}
