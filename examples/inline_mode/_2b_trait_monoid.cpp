#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_std.hpp>
#include <catch2/catch_test_macros.hpp>
#include <list>
#include <print>
#include <string>
#include <vector>

#ifdef __cpp_lib_ranges_fold

namespace anyxx {

//
}  // namespace anyxx

namespace example_2b {

TRAIT_EX(monoid,
         (ANY_METHOD_DEFAULTED(anyxx::self, id, (), const,
                               []() { return T{}; }),
          ANY_OP_DEFAULTED(anyxx::self, +, op, (anyxx::self const&), const,
                           [&x](auto const& r) {
                             std::println("op-default {}", typeid(T).name());
                             auto self = anyxx::trait_as<monoid>(x);
                             return self | (std::vector{anyxx::trait_as<monoid>(
                                               r)});  // NOLINT
                           }),
          ANY_OP_DEFAULTED(
              anyxx::self, |, concat,
              ((anyxx::any_forward_range<anyxx::self, anyxx::self,
                                         anyxx::const_observer> const&)),
              const,
              [&x](const auto& r) {
                std::println("concat-default {}", typeid(T).name());
                auto self = anyxx::trait_as<monoid>(x);
                return std::ranges::fold_left(
                    r | std::views::transform([](auto const& y) {
                      return anyxx::trait_as<monoid>(y);
                    }),
                    self,
                    [&](auto const& m1, auto const& m2) { return m1 + m2; });
              }),
          ANY_METHOD_DEFAULTED(bool, equal_to, (anyxx::self const&), const,
                               ([&x](auto const& r) { return x == r; }))),
         (template <typename Box> friend bool operator==(
             anyxx::any<Box, monoid> const& l,
             anyxx::any<Box, monoid> const& r) { return l.equal_to(r); }))

template <typename Box = anyxx::value>
using any_monoid = anyxx::any<Box, monoid>;

}  // namespace example_2b

ANY_MODEL_MAP((int), example_2b::monoid) {
  static int concat(int self, auto const& r) {
    std::println("concat {}", typeid(int).name());
    return std::ranges::fold_left(r, self,
                                  [&](int m1, int m2) { return m1 + m2; });
  };
};

ANY_MODEL_MAP((std::string), example_2b::monoid) {
  static std::string op(std::string const& self, std::string const& r) {
    std::println("op {}", typeid(std::string).name());
    return self + r;
  };
};

namespace example_2b {

template <anyxx::is_any Monoid>
void test_monoid(Monoid const& m, std::ranges::forward_range auto const& r) {
  auto id = m.id();
  using type_1 = decltype(m + id + m);
  using type_2 = decltype(m + (m + id));
  static_assert(std::same_as<type_1, type_2>);
  static_assert(std::same_as<type_1, Monoid>);
  auto c1 = m + id + m == m + m + id;
  CHECK(c1);
  auto c2 = (m | r) ==
            std::ranges::fold_left(
                r, m, [&](Monoid const& m1, [[maybe_unused]] Monoid const& m2) {
                  return m1 + m2;
                });
  CHECK(c2);
}

anyxx::any_forward_range<any_monoid<anyxx::value>, any_monoid<anyxx::value>,
                         anyxx::value>
make_a_range(bool use_list) {
  using namespace std::string_literals;
  if (use_list)
    return std::list<any_monoid<anyxx::value>>{{"2"s}, {"3"s}};
  else
    return std::vector<any_monoid<anyxx::value>>{{"2"s}, {"3"s}};
}

}  // namespace example_2b
//
TEST_CASE("example 2b monoid simple") {
  example_2b::any_monoid<anyxx::val<int>> x{2};
  example_2b::any_monoid<anyxx::val<int>> y{x};
  example_2b::any_monoid<anyxx::val<int>> z = y;
  CHECK(static_cast<int>(z) == 2);
  example_2b::any_monoid<anyxx::val<int>> a{std::move(x)};
  CHECK(static_cast<int>(a) == 2);
  static_assert(anyxx::is_erased_data<decltype(x)::erased_data_t>);
  static_assert(anyxx::is_any<decltype(x)>);
  static_assert(anyxx::moveable_from<decltype(x)::erased_data_t,
                                     decltype(y)::erased_data_t>);
  static_assert(!anyxx::borrowable_from<decltype(x)::erased_data_t,
                                        decltype(y)::erased_data_t>);
}

TEST_CASE("example 2b monoid a") {
  using namespace example_2b;
  using namespace std::string_literals;
  using namespace anyxx;

  test_monoid<any<val<int>, monoid>>(
      trait_as<monoid>(1), std::vector<any<val<int>, monoid>>{{2}, {3}});
}
TEST_CASE("example 2b monoid b") {
  using namespace example_2b;
  using namespace std::string_literals;
  using namespace anyxx;

  test_monoid<val<std::string>::as<monoid>>(
      trait_as<monoid>("1"s),
      std::vector<val<std::string>::as<monoid>>{{"2"s}, {"3"s}});
}
TEST_CASE("example 2b monoid c") {
  using namespace example_2b;
  using namespace std::string_literals;
  using namespace anyxx;

  test_monoid<any_monoid<anyxx::value>>("1"s, make_a_range(true));
}
TEST_CASE("example 2b monoid d") {
  using namespace example_2b;
  using namespace std::string_literals;
  using namespace anyxx;

  test_monoid<any_monoid<anyxx::value>>("1"s, make_a_range(false));
}

#endif
