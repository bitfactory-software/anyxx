#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/v23/anys/ranges.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <map>
#include <print>

namespace example_3_view {

static_assert(std::is_pointer_v<const int*>);
// static_assert(std::same_as<std::remove_const_t<const int*>, int*>);
//
// static_assert(std::same_as<anyxx::unerased<anyxx::nullable_val<>, const
// int*>, int*>);

const int* inc(const int*& p) { return ++p; }

template <typename T>
concept incrementable = requires(T x) {
  { ++x } -> std::convertible_to<anyxx::map_return<T, anyxx::self&>>;
};
static_assert(incrementable<const int*>);
static_assert(
    std::same_as<anyxx::map_return<const int*, anyxx::self&>, const int*&>);

anyxx::any<anyxx::view, anyxx::any_forward_iterator<int const>> a_view(
    int use) {
  static std::vector<int> v = {1, 2, 3};
  static std::list<int> l = {4, 5, 6};
  static int s = 7;
  if (use == 0) {
    return std::views::all(v);
  } else if (use == 1) {
    return std::views::all(l);
  } else {
    return std::views::single(s);
  }
}

}  // namespace example_3_view
TEST_CASE("example3 a_view") {
  int i = 1;
  for (auto x : example_3_view::a_view(0)) {
    CHECK(x == i++);
  }
  for (auto x : example_3_view::a_view(1)) {
    CHECK(x == i++);
  }
  for(auto x : example_3_view::a_view(3)) {
      CHECK(x == i++);
  }
  CHECK(i == 8);
}

namespace example_3_view {
anyxx::any<anyxx::view, anyxx::any_bidirectional_iterator<std::pair<int, int>,
                                                          std::pair<int, int>>>
a_bidirectional_view([[maybe_unused]] bool use_map) {
  static std::vector<std::pair<int, int>> v = {{1, 1}, {2, 2}, {3, 3}};
  static std::map<int, int> m = {{4, 4}, {5, 5}, {6, 6}};
  if (use_map)
    return std::views::all(m);
  else
    return std::views::all(v);
}
}  // namespace example_3_view
TEST_CASE("example3 a_bidirectional_view") {
  int i = 1;
  for (auto x : example_3_view::a_bidirectional_view(false)) {
    CHECK(x.second == i++);
  }
  for (auto x : example_3_view::a_bidirectional_view(true)) {
    CHECK(x.second == i++);
  }
  CHECK(i == 7);
}

namespace example_3_view {
anyxx::any<anyxx::sized_view,
           anyxx::any_random_access_iterator<std::pair<int, int>>>
a_sized_view() {
  static std::vector<std::pair<int, int>> v = {{1, 1}, {2, 2}, {3, 3}};
  return std::views::all(v);
}
}  // namespace example_3_view
TEST_CASE("example3 a_sized_view") {
  int i = 1;
  for (auto x : example_3_view::a_sized_view()) {
    CHECK(x.second == i++);
  }
  CHECK(i == 4);
  auto v = example_3_view::a_sized_view();
  CHECK((std::ranges::size(v) == 3));
}
