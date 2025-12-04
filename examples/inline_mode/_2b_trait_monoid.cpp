#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace example_2b {
TRAIT(monoid,
      (TRAIT_METHOD(monoid<T>, op, (monoid<T> const&), const,
                    [](const auto& value, monoid<T> const r) {
                      return monoid<T>{}.concat(std::vector{monoid{value}, r});
                    }),
       TRAIT_METHOD(monoid<T>, concat, (const auto&), const,
                    []([[maybe_unused]] const auto& value, const auto& r) {
                      return std::ranges::fold_right(
                          r, monoid<T>{},
                          [&](auto m1, auto m2) { return m1.op(m2); });
                    }),
       TRAIT_METHOD(monoid<T>, id, (), const,
                    []([[maybe_unused]] const auto& value) {
                      return monoid<T>{};
                    })))

template <typename T>
inline auto operator==(monoid<T> const& lhs, monoid<T> const& rhs) {
  return lhs.value_ == rhs.value_;
};

template <>
struct monoid_trait<int> : monoid_trait_default<int> {
  static monoid<int> concat([[maybe_unused]] int self, auto const& r) {
    return std::ranges::fold_right(r, 0,
                                   [&](auto m1, auto m2) { return m1 + m2; });
  };
};

template <>
struct monoid_trait<std::string> : monoid_trait_default<std::string> {
  static monoid<std::string> op(std::string const& self, monoid<std::string> r) {
    return self + static_cast<std::string>(r);
  };
};

template <typename M, typename R>
void test_monoid(monoid<M> m, R r)
  requires std::ranges::range<R> &&
           std::same_as<typename R::value_type, monoid<M>>
{
  CHECK(m.op(monoid<M>{}).op(m) == m.op(m).op(monoid<M>{}));
  CHECK(m.id() == monoid<M>{});
  CHECK(m.concat(r) ==
        std::ranges::fold_right(r, monoid<M>{},
                                [&](auto m1, auto m2) { return m1.op(m2); }));
}

}  // namespace example_2

TEST_CASE("example 2b monoid ") {
  using namespace example_2b;
  using namespace std::string_literals;
  test_monoid(monoid{1}, std::vector{monoid{2}, monoid{3}});
  test_monoid(monoid{"1"s}, std::vector{monoid{"2"s}, monoid{"3"s}});
}
