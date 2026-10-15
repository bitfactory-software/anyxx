#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v23/anys/range.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <list>
#include <print>

namespace example_3_view {

anyxx::any_<anyxx::view, anyxx::any_forward_iterator<int>> a_view(bool use_list) {
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
