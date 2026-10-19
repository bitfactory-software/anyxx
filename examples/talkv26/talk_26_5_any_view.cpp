#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/v26/anys/ranges.hpp>
#include <catch2/catch_test_macros.hpp>
#include <list>
#include <ranges>
#include <vector>

namespace {

using namespace anyxx;

class container {
  std::vector<int> v{{1}, {2}, {3}, {4}, {5}};
  std::list<int> l{{1}, {2}, {3}, {4}, {5}};
  int s{1};

 public:
  any<view, bidirectional, int const> get_view(int i) {
    if (i == 0) {
      return v;
    } else if (i == 1) {
      return l;
    } else {
      return std::views::single(s);
    }
  }
};

static_assert(std::ranges::bidirectional_range<any<view, bidirectional, int const>>);
static_assert(std::ranges::view<any<view, bidirectional, int const>>);

TEST_CASE("talk_26_5_any_view") {
  using namespace std::views;
  container c;
  for (auto i : iota(0, 3)) {
    auto v = c.get_view(i);
    auto it = v.begin();
    ++it;
    --it;
    CHECK(*it == 1);
    CHECK(it == v.begin());
    if (i < 2) {
      CHECK(++it != v.end());
    } else {
      CHECK(++it == v.end());
    }
  }
}

}  // namespace
