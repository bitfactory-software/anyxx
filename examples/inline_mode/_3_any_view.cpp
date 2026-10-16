#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/v23/anys/ranges.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <map>
#include <print>

namespace example_3_view {

anyxx::any_<anyxx::view, anyxx::any_forward_iterator<int>> a_view(
    bool use_list) {
  static std::vector<int> v = {1, 2, 3};
  static std::list<int> l = {4, 5, 6};
  if (use_list)
    return std::views::all(l);
  else
    return std::views::all(v);
}

}  // namespace example_3_view
TEST_CASE("example3 a_view") {
  int i = 1;
  for (auto x : example_3_view::a_view(false)) {
    CHECK(x == i++);
  }
  for (auto x : example_3_view::a_view(true)) {
    CHECK(x == i++);
  }
  CHECK(i == 7);
}

namespace example_3_view {
anyxx::any_<anyxx::view, anyxx::any_bidirectional_iterator<std::pair<int, int>,
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
anyxx::any_<anyxx::sized_view,
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
