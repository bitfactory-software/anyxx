//#include <array>
#include <bit_factory/anyxx/v26/anys/ranges.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ranges>

#if defined(__GNUC__) and !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

using namespace anyxx;

static_assert(has_deduced_typenames<input_iterator, int>);
static_assert(has_identifier(compute_deduced_typenames<input_iterator, int>()));
static_assert(has_identifier(^^input_iterator<declaration, declaration, int>::typenames));
static_assert(std::same_as<deduced_typenames<input_iterator, int>, input_iterator<declaration, declaration, int>::typenames>);


static_assert(std::input_iterator<any_<input_iterator, int>>);
static_assert(std::forward_iterator<any_<forward_iterator, int>>);
static_assert(std::bidirectional_iterator<any_<bidirectional_iterator, int>>);
static_assert(std::random_access_iterator<any_<random_access_iterator, int>>);
static_assert(std::contiguous_iterator<any_<contiguous_iterator, int>>);
static_assert(std::input_iterator<any_<input_iterator, int const>>);
static_assert(std::forward_iterator<any_<forward_iterator, int const>>);
static_assert(std::bidirectional_iterator<any_<bidirectional_iterator, int const>>);
static_assert(std::random_access_iterator<any_<random_access_iterator, int const>>);
static_assert(std::contiguous_iterator<any_<contiguous_iterator, int const>>);

static_assert(std::sentinel_for<any_<sentinel, int, int&>, any_<input_iterator, int, int&>>);
static_assert(std::sentinel_for<any_<sentinel, int, int&>, any_<forward_iterator, int, int&>>);
static_assert(std::sentinel_for<any_<sentinel, int, int&>, any_<bidirectional_iterator, int, int&>>);
static_assert(std::sentinel_for<any_<sentinel, int, int&>, any_<random_access_iterator, int, int&>>);
static_assert(std::sentinel_for<any_<sentinel, int, int&>, any_<contiguous_iterator, int, int&>>);

static_assert(std::sentinel_for<any_<sentinel, int const, int const&>, any_<input_iterator, int const, int const&>>);
static_assert(std::sentinel_for<any_<sentinel, int const, int const&>, any_<forward_iterator, int const, int const&>>);
static_assert(std::sentinel_for<any_<sentinel, int const, int const&>, any_<bidirectional_iterator, int const, int const&>>);
static_assert(std::sentinel_for<any_<sentinel, int const, int const&>, any_<random_access_iterator, int const, int const&>>);
static_assert(std::sentinel_for<any_<sentinel, int const, int const&>, any_<contiguous_iterator, int const, int const&>>);

static_assert(std::ranges::range<any_<view, input, int>>);
static_assert(std::ranges::range<any_<view, forward, int>>);
static_assert(std::ranges::range<any_<view, bidirectional, int>>);
static_assert(std::ranges::range<any_<view, random_access, int>>);
static_assert(std::ranges::range<any_<view, contiguous, int>>);

static_assert(std::ranges::range<any_<view, input, int, int>>);
static_assert(std::ranges::range<any_<view, forward, int, int>>);
static_assert(std::ranges::range<any_<view, bidirectional, int, int>>);
static_assert(std::ranges::range<any_<view, random_access, int, int>>);
static_assert(std::ranges::range<any_<view, contiguous, int, int>>);

static_assert(std::ranges::range<any_<view, input, int, int&>>);
static_assert(std::ranges::range<any_<view, forward, int, int&>>);
static_assert(std::ranges::range<any_<view, bidirectional, int, int&>>);
static_assert(std::ranges::range<any_<view, random_access, int, int&>>);
static_assert(std::ranges::range<any_<view, contiguous, int, int&>>);

static_assert(std::ranges::range<any_<view, input, int, int const&>>);
static_assert(std::ranges::range<any_<view, forward, int, int const&>>);
static_assert(std::ranges::range<any_<view, bidirectional, int, int const&>>);
static_assert(std::ranges::range<any_<view, random_access, int, int const&>>);
static_assert(std::ranges::range<any_<view, contiguous, int, int const&>>);

static_assert(std::ranges::range<any_<view, input, int const>>);
static_assert(std::ranges::range<any_<view, forward, int const>>);
static_assert(std::ranges::range<any_<view, bidirectional, int const>>);
static_assert(std::ranges::range<any_<view, random_access, int const>>);
static_assert(std::ranges::range<any_<view, contiguous, int const>>);

static_assert(std::ranges::range<any_<view, input, int const, int const>>);
static_assert(std::ranges::range<any_<view, forward, int const, int const>>);
static_assert(std::ranges::range<any_<view, bidirectional, int const, int const>>);
static_assert(std::ranges::range<any_<view, random_access, int const, int const>>);
static_assert(std::ranges::range<any_<view, contiguous, int const, int const>>);

namespace {
struct a_struct {};
}  // namespace
static_assert(std::ranges::sized_range<any_<sized_view, contiguous, a_struct>>);
static_assert(has_trait_facade_decorator<sized_view, contiguous, a_struct>);

