#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#ifdef __cpp_lib_ranges_fold

namespace example_2b {

TRAIT(monoid,
      (ANY_OP_DEFAULTED(monoid_trait<T>, +, op, (monoid_trait<T> const&), const,
                        [&x](monoid_trait<T> const r) {
                          auto self = anyxx::trait_as<monoid>(x);
                          return self | (std::vector{r});  // NOLINT
                        }),
       ANY_OP_DEFAULTED(monoid_trait<T>, |, concat, (const auto&), const,
                        [&x](const auto& r) {
                          auto self = anyxx::trait_as<monoid>(x);
                          return std::ranges::fold_right(
                              r, self, [&](auto const& m1, auto const& m2) {
                                return m1 + m2;
                              });
                        })))

template <typename T>
inline auto operator==(monoid_trait<T> const& lhs, monoid_trait<T> const& rhs) {
  return *lhs == *rhs;
};

}  // namespace example_2b

ANY_MODEL_MAP((int), example_2b::monoid) {
  static monoid_trait<int> concat(int self, auto const& r) {
    return monoid_trait<int>{std::ranges::fold_right(
        r, self, [&](auto m1, auto m2) { return *m1 + m2; })};
  };
};

ANY_MODEL_MAP((std::string), example_2b::monoid) {
  static monoid_trait<std::string> op(std::string const& self,
                                      monoid_trait<std::string> const& r) {
    return self + *r;
  };
};

namespace example_2b {

template <typename M, typename R>
void test_monoid(monoid_trait<M> const& m, R r)
  requires std::ranges::range<R> &&
           std::same_as<typename R::value_type, monoid_trait<M>>
{
  using type_1 = decltype(m + (monoid_trait<M>{}) + m);
  using type_2 = decltype(m + (m + (monoid_trait<M>{})));
  static_assert(std::same_as<type_1, type_2>);
  static_assert(std::same_as<type_1, monoid_trait<M>>);
  auto c1 = m + monoid_trait<M>{} + m == m + m + monoid_trait<M>{};
  CHECK(c1);
  auto c2 = (m | r) ==
            std::ranges::fold_right(
                r, m, [&](auto const& m1, auto const& m2) { return m1 + m2; });
  CHECK(c2);
}

}  // namespace example_2b

TEST_CASE("example 2b monoid ") {
  using namespace example_2b;
  using namespace std::string_literals;
  using namespace anyxx;
  test_monoid(trait_as<monoid>(1), std::vector<monoid_trait<int>>{{2}, {3}});
  test_monoid(trait_as<monoid>("1"s),
              std::vector<monoid_trait<std::string>>{{"2"s}, {"3"s}});
}

#endif