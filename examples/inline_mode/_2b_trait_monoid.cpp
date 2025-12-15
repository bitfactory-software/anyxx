#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#ifdef __cpp_lib_ranges_fold

namespace example_2b {
TRAIT(monoid,
      (ANY_METHOD_DEFAULTED(as_monoid<T>, op, (as_monoid<T> const&), const,
                            [x](as_monoid<T> const r) {
                              return as_monoid<T>{}.concat(
                                  std::vector{as_monoid<T>{x}, r});  // NOLINT
                            }),
       ANY_METHOD_DEFAULTED(as_monoid<T>, concat, (const auto&), const,
                            []([[maybe_unused]] const auto& r) {
                              return std::ranges::fold_right(
                                  r, as_monoid<T>{},
                                  [&](auto const& m1, auto const& m2) {
                                    return m1.op(m2);
                                  });
                            }),
       ANY_METHOD_DEFAULTED(as_monoid<T>, id, (), const,
                            []() { return as_monoid<T>{}; })))

template <typename T>
inline auto operator==(as_monoid<T> const& lhs, as_monoid<T> const& rhs) {
  return *lhs == *rhs;
};

}  // namespace example_2b

ANY_MODEL_MAP((int), example_2b::monoid) {
  static as_monoid<int> concat([[maybe_unused]] int self, auto const& r) {
    return as_monoid<int>{std::ranges::fold_right(
        r, 0, [&](auto m1, auto m2) { return *m1 + m2; })};
  };
};

ANY_MODEL_MAP((std::string), example_2b::monoid) {
  static as_monoid<std::string> op(std::string const& self,
                                   as_monoid<std::string> const& r) {
    return as_monoid<std::string>{self + *r};
  };
};

namespace example_2b {

template <typename M, typename R>
void test_monoid(as_monoid<M> const& m, R r)
  requires std::ranges::range<R> &&
           std::same_as<typename R::value_type, as_monoid<M>>
{
  using type_1 = decltype(m.op(as_monoid<M>{}).op(m));
  using type_2 = decltype(m.op(m).op(as_monoid<M>{}));
  static_assert(std::same_as<type_1, type_2>);
  static_assert(std::same_as<type_1, as_monoid<M>>);
  auto c1 = m.op(as_monoid<M>{}).op(m) == m.op(m).op(as_monoid<M>{});
  CHECK(c1);
  auto c2 = m.concat(r) ==
      std::ranges::fold_right(
          r, as_monoid<M>{},
          [&](auto const& m1, auto const& m2) { return m1.op(m2); });
  CHECK(c2);
  auto c3 = m.id() == as_monoid<M>{};
  CHECK(c3);
}

}  // namespace example_2b

TEST_CASE("example 2b monoid ") {
  using namespace example_2b;
  using namespace std::string_literals;
  test_monoid(as_monoid<int>{1}, std::vector<as_monoid<int>>{{2}, {3}});
  test_monoid(as_monoid<std::string>{"1"s},
              std::vector<as_monoid<std::string>>{{"2"s}, {"3"s}});
}

#endif