#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_std.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

#ifdef __cpp_lib_ranges_fold

namespace anyxx {

//
}  // namespace anyxx

namespace example_2b {

ANY_EX(monoid,
       (ANY_METHOD_DEFAULTED(anyxx::self, id, (), const, []() { return T{}; }),
        ANY_OP_DEFAULTED(anyxx::self, +, op, (anyxx::self const&), const,
                         [&x](auto const& r) {
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
              auto self = anyxx::trait_as<monoid>(x);
              return std::ranges::fold_left(
                  r | std::views::transform(
                          [](auto y) { return anyxx::trait_as<monoid>(y); }),
                  self,
                  [&](auto const& m1, auto const& m2) { return m1 + m2; });
            }),
        ANY_METHOD_DEFAULTED(bool, equal_to, (anyxx::self const&), const,
                             ([&x](auto const& r) { return x == r; }))),
       , , (friend bool operator==(any_t const& l, any_t const& r) {
         return l.equal_to(r);
       }))

}  // namespace example_2b

ANY_MODEL_MAP((int), example_2b::monoid) {
  static int concat(int self, auto const& r) {
    return std::ranges::fold_left(r, self,
                                  [&](int m1, int m2) { return m1 + m2; });
  };
};

ANY_MODEL_MAP((std::string), example_2b::monoid) {
  static std::string op(std::string const& self, std::string const& r) {
    return self + r;
  };
};

namespace example_2b {

template <anyxx::is_any Monoid>
void test_monoid(
    Monoid const& m,
    anyxx::any_forward_range<Monoid, Monoid, anyxx::const_observer> r) {
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

}  // namespace example_2b
//
TEST_CASE("example 2b monoid ") {
  using namespace example_2b;
  using namespace std::string_literals;
  using namespace anyxx;
  auto x = trait_as<monoid>(1);
  using x_t = decltype(x);

  test_monoid<monoid_trait<int>>(trait_as<monoid>(1),
                                 std::vector<monoid_trait<int>>{{2}, {3}});

  test_monoid<monoid_trait<std::string>>(
      trait_as<monoid>("1"s),
      std::vector<monoid_trait<std::string>>{{"2"s}, {"3"s}});

  test_monoid<monoid<anyxx::value>>(
      "1"s, std::vector<monoid<anyxx::value>>{{"2"s}, {"3"s}});
}

#endif
