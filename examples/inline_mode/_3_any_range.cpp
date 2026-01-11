#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_std.hpp>
#include <catch2/catch_test_macros.hpp>
#include <list>
#include <print>

namespace example_3 {

anyxx::any_forward_range<int, int> a_range(bool use_list) {
  static std::vector<int> v = {1, 2, 3};
  static std::list<int> l = {4, 5, 6};
  if (use_list)
    return l;
  else
    return v;
}

anyxx::any_forward_range<int, int, anyxx::value> a_range_value(bool use_list) {
  if (use_list)
    return std::list<int>{4, 5, 6};
  else
    return std::vector<int>{1, 2, 3};
}

ANY(stringable,
    (ANY_METHOD_DEFAULTED(std::string, to_string, (), const,
                          [&x]() { return std::format("{}", x); })),
    , )

}  // namespace example_3

TEST_CASE(
    "example 3 any_forward_iterator (concrete value_type, erased iterator)") {
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
    CHECK(!(b == e));
    int x = 0;
    for (auto i = b; i != e; ++i) {
      CHECK(*i == v[x++]);
    }
    CHECK(x == 3);
    any_forward_range<int, int> r{v};
    x = 0;
    for (auto i : r) CHECK(i == v[x++]);
    CHECK(x == 3);
  }
  {
    int x = 1;
    for (auto i : a_range(false)) CHECK(i == x++);
    for (auto i : a_range(true)) CHECK(i == x++);
    CHECK(x == 7);
  }
  {
    int x = 1;
    for (auto i : a_range_value(false)) CHECK(i == x++);
    for (auto i : a_range_value(true)) CHECK(i == x++);
    CHECK(x == 7);
  }
}

TEST_CASE(
    "example 3 any_forward_iterator (concrete value_type, concrete iterator)") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v{1, 2, 3};
    any_forward_range_trait<v_t const &, int, int> r{v};
    int x = 0;
    for (auto i : r) CHECK(i == v[x++]);
    CHECK(x == 3);
  }
}

TEST_CASE("example 3 any_forward_iterator (any value_type, erased iterator)") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v{1, 2, 3};
    any_forward_range<stringable<anyxx::value>, stringable<anyxx::value>> r{v};
    int x = 0;
    for (auto i : r) CHECK(i.to_string() == std::to_string(v[x++]));
    CHECK(x == 3);
  }
}

TEST_CASE(
    "example 3 any_forward_iterator (any value_type, concrete iterator) only "
    "theory, not praxis relevant") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v{1, 2, 3};
    any_forward_range_trait<v_t const &, stringable<anyxx::value>,
                            stringable<anyxx::value>>
        r{v};
    int x = 0;
    for (auto i : r) {
      CHECK(i.to_string() == std::to_string(v[x++]));
    }
    CHECK(x == 3);
  }
}

TEST_CASE("example 3 transform unerase") {
  using namespace anyxx;
  using namespace std::string_literals;
  using namespace example_3;

  using v_t = std::vector<int>;
  {
    v_t v{1, 2, 3};
    any_forward_range_trait<v_t const &, stringable<anyxx::value>,
                            stringable<anyxx::value>>
        r{v};
    int x = 0;
    for (auto i :
         std::views::transform(r, [](stringable<anyxx::value> const &v) -> int {
           return *anyxx::unerase_cast<int>(v);
         })) {
      std::println("{}", i);
      CHECK(i == ++x);
    }
    CHECK(x == 3);
  }
}
