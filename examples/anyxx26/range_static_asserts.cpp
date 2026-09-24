#include <array>
#include <bit_factory/v26/anys/range.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ranges>

#if defined(__GNUC__) and !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

using namespace anyxx26;

static_assert(has_deduced_typenames<input_iterator, int>);
static_assert(has_identifier(compute_deduced_typenames<input_iterator, int>()));
static_assert(has_identifier(^^input_iterator<declaration, declaration, int>::typenames));
static_assert(std::same_as<deduced_typenames<input_iterator, int>, input_iterator<declaration, declaration, int>::typenames>);


static_assert(std::input_iterator<any<input_iterator, int>>);
static_assert(std::forward_iterator<any<forward_iterator, int>>);
static_assert(std::bidirectional_iterator<any<bidirectional_iterator, int>>);
static_assert(std::random_access_iterator<any<random_access_iterator, int>>);
static_assert(std::contiguous_iterator<any<contiguous_iterator, int>>);
static_assert(std::input_iterator<any<input_iterator, int const>>);
static_assert(std::forward_iterator<any<forward_iterator, int const>>);
static_assert(std::bidirectional_iterator<any<bidirectional_iterator, int const>>);
static_assert(std::random_access_iterator<any<random_access_iterator, int const>>);
static_assert(std::contiguous_iterator<any<contiguous_iterator, int const>>);

static_assert(std::sentinel_for<any<sentinel, int, int&>, any<input_iterator, int, int&>>);
static_assert(std::sentinel_for<any<sentinel, int, int&>, any<forward_iterator, int, int&>>);
static_assert(std::sentinel_for<any<sentinel, int, int&>, any<bidirectional_iterator, int, int&>>);
static_assert(std::sentinel_for<any<sentinel, int, int&>, any<random_access_iterator, int, int&>>);
static_assert(std::sentinel_for<any<sentinel, int, int&>, any<contiguous_iterator, int, int&>>);

static_assert(std::sentinel_for<any<sentinel, int const, int const&>, any<input_iterator, int const, int const&>>);
static_assert(std::sentinel_for<any<sentinel, int const, int const&>, any<forward_iterator, int const, int const&>>);
static_assert(std::sentinel_for<any<sentinel, int const, int const&>, any<bidirectional_iterator, int const, int const&>>);
static_assert(std::sentinel_for<any<sentinel, int const, int const&>, any<random_access_iterator, int const, int const&>>);
static_assert(std::sentinel_for<any<sentinel, int const, int const&>, any<contiguous_iterator, int const, int const&>>);

static_assert(std::ranges::range<any<view, input, int>>);
static_assert(std::ranges::range<any<view, forward, int>>);
static_assert(std::ranges::range<any<view, bidirectional, int>>);
static_assert(std::ranges::range<any<view, random_access, int>>);
static_assert(std::ranges::range<any<view, contiguous, int>>);

static_assert(std::ranges::range<any<view, input, int, int>>);
static_assert(std::ranges::range<any<view, forward, int, int>>);
static_assert(std::ranges::range<any<view, bidirectional, int, int>>);
static_assert(std::ranges::range<any<view, random_access, int, int>>);
static_assert(std::ranges::range<any<view, contiguous, int, int>>);

static_assert(std::ranges::range<any<view, input, int, int&>>);
static_assert(std::ranges::range<any<view, forward, int, int&>>);
static_assert(std::ranges::range<any<view, bidirectional, int, int&>>);
static_assert(std::ranges::range<any<view, random_access, int, int&>>);
static_assert(std::ranges::range<any<view, contiguous, int, int&>>);

static_assert(std::ranges::range<any<view, input, int, int const&>>);
static_assert(std::ranges::range<any<view, forward, int, int const&>>);
static_assert(std::ranges::range<any<view, bidirectional, int, int const&>>);
static_assert(std::ranges::range<any<view, random_access, int, int const&>>);
static_assert(std::ranges::range<any<view, contiguous, int, int const&>>);

static_assert(std::ranges::range<any<view, input, int const>>);
static_assert(std::ranges::range<any<view, forward, int const>>);
static_assert(std::ranges::range<any<view, bidirectional, int const>>);
static_assert(std::ranges::range<any<view, random_access, int const>>);
static_assert(std::ranges::range<any<view, contiguous, int const>>);

static_assert(std::ranges::range<any<view, input, int const, int const>>);
static_assert(std::ranges::range<any<view, forward, int const, int const>>);
static_assert(std::ranges::range<any<view, bidirectional, int const, int const>>);
static_assert(std::ranges::range<any<view, random_access, int const, int const>>);
static_assert(std::ranges::range<any<view, contiguous, int const, int const>>);

