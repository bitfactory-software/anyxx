#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>

namespace example_2 {

using namespace std;

TRAIT(stringable,
      (TRAIT_METHOD(std::string, to_string, (), const, [](const auto& value) {
        return std::format("{}", value);
      })))

template <>
struct stringable_trait<bool> {
  static std::string to_string(bool const& value) {
    return value ? "wahr" : "falsch";
  };
};

template <>
struct stringable_trait<double> {
  static std::string to_string(const double& value) {
    return std::format("{:6.3}", value);
  }
};

template <typename V>
std::string print_(stringable<V> s) {
  return s.to_string() + "\n";
}
template <typename V>
auto print(V const& s)
//  requires stringable_trait<V>::is_defined
{
  return print_(stringable<V>{s});
}

}  // namespace example_2

template <class V>
concept is_print_callable = requires(V v) {
  { print(v) } -> std::same_as<std::string>;
};

TEST_CASE("example 2a stringable") {
  using namespace example_2;
  CHECK(print(true) == "wahr\n");
  CHECK(print(3.14) == "  3.14\n");
  CHECK(print(42) == "42\n");
  //  static_assert(!is_print_callable<int>);
  // print(42);  // remove comment to see the compilation error!
}

namespace example_2 {
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
  //  static monoid<int> op(int self, monoid<int> r) { return self + r; };
  // static monoid<int> id([[maybe_unused]] int self) { return {}; };
  static monoid<int> concat([[maybe_unused]] int self, auto const& r) {
    return std::ranges::fold_right(r, 0,
                                   [&](auto m1, auto m2) { return m1 + m2; });
  };
};

template <>
struct monoid_trait<std::string> : monoid_trait_default<std::string> {
  static monoid<std::string> op(std::string const& self, monoid<string> r) {
    return self + static_cast<std::string>(r);
  };
  // static monoid<std::string> id([[maybe_unused]] std::string const& self) {
  //   return {};
  // };
  // static monoid<std::string> concat([[maybe_unused]] std::string const& self,
  //                                   auto const& r) {
  //   using namespace std::string_literals;
  //   return std::ranges::fold_right(r, ""s,
  //                                  [&](auto m1, auto m2) { return m1.op(m2);
  //                                  });
  // };
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
  using namespace example_2;
  using namespace std::string_literals;
  test_monoid(monoid{1}, std::vector{monoid{2}, monoid{3}});
  test_monoid(monoid{"1"s}, std::vector{monoid{"2"s}, monoid{"3"s}});
}
