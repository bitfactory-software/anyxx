#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_std.hpp>
#include <catch2/catch_test_macros.hpp>

namespace example_3 {

anyxx::any_forward_range<int, int> a_range(bool use_list) {
  static std::vector<int> v = {1, 2, 3};
  static std::list<int> l = {4, 5, 6};
  if (use_list)
    return l;
  else
    return v;
}

}  // namespace example_3

TEST_CASE("example 3a any_forward_iterator") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  using it = v_t::iterator;
  {
    v_t v;
    any_forward_iterator<int, int> b{v.begin()};
    any_forward_iterator<int, int> e{v.end()};
    CHECK(b == e);
    CHECK(!(b != e));
    static_assert(std::movable<any_forward_iterator<int, int>>);
    static_assert(
        std::same_as<decltype(++b), any_forward_iterator<int, int> &>);
    static_assert(std::forward_iterator<any_forward_iterator<int, int>>);
  }
  {
    v_t v{1, 2, 3};
    any_forward_iterator<int, int> b{v.begin()};
    any_forward_iterator<int, int> e{v.end()};
    CHECK(b != e);
    int x = 0;
    for (auto i = b; i != e; ++i) CHECK(*i == v[x++]);
    any_forward_range<int, int> r{v};
    x = 0;
    for (auto i : r) CHECK(i == v[x++]);
  }
  {
    int x = 1;
    for (auto i : a_range(false)) CHECK(i == x++);
    for (auto i : a_range(true)) CHECK(i == x++);
  }
}
