#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_std.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

#ifdef __cpp_lib_ranges_fold

namespace anyxx {

template <typename A>
concept is_any_self_forward_range =
    is_any<A> && std::ranges::forward_range<A> &&
    std::same_as<std::ranges::range_value_t<A>, self>;
//
template <typename Any, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_jacket_param<Any, AnyForwardRange const&> {
  using type =
      anyxx::any_forward_range<typename Any, typename Any,
                               typename AnyForwardRange::erased_data_t,
                               typename AnyForwardRange::dispatch_t> const&;
};
//
template <typename AnyConstObserver, typename AnyMutableObserver,
          typename AnyValue, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_v_table_param<AnyConstObserver, AnyMutableObserver, AnyValue,
                               typename AnyForwardRange const&> {
  using type =
      anyxx::any_forward_range<AnyValue, AnyValue,
                               typename AnyForwardRange::erased_data_t,
                               typename AnyForwardRange::dispatch_t> const&;
};
//
template <typename Concrete, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct v_table_to_map<Concrete, AnyForwardRange const&> {
  static auto forward(auto const& any_range) {
    return std::views::transform(any_range, [](auto const& any) {
      return *unerase_cast<Concrete>(any);
    });
  }
};
//
template <typename T, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_map_param<T, AnyForwardRange const&> {
  using type =
      anyxx::any_forward_range<T, T, typename AnyForwardRange::erased_data_t,
                               typename AnyForwardRange::dispatch_t> const&;
};
//
template <typename Traited, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct forward_trait_to_map<Traited, AnyForwardRange const&> {
  static auto forward(auto any_range) {
    return std::views::transform(
        any_range, [](auto const& any) -> Traited { return any; });
  }
};
//
}  // namespace anyxx
//
//(ANY_OP_DEFAULTED(anyxx::self, +, op, (anyxx::self const&), const,
//                  [&x](auto const& r) {
//                    auto self = anyxx::trait_as<monoid>(x);
//                    return self | (std::vector{anyxx::trait_as<monoid>(
//                                      r)});  // NOLINT
//                  }),
// ANY_OP_DEFAULTED(anyxx::self, |, concat,
//                  ((anyxx::any_forward_range<anyxx::self, anyxx::self,
//                                             anyxx::value> const&)),
//                  const,
//                  [&x](const auto& r) {
//                    auto self = anyxx::trait_as<monoid>(x);
//                    return std::ranges::fold_left(
//                        r | std::views::transform([](auto y) {
//                          return anyxx::trait_as<monoid>(y);
//                        }),
//                        self, [&](auto const& m1, auto const& m2) {
//                          return m1 + m2;
//                        });
//                  }),
// ANY_OP_DEFAULTED(bool, ==, equal, (anyxx::self const&), const,
//                  ([&x](auto const& r) { return x == r; }))),
//, )
//
//

namespace example_2b {

ANY(monoid,
    (ANY_OP_DEFAULTED(anyxx::self, +, op, (anyxx::self const&), const,
                      [&x](auto const& r) {
                        auto self = anyxx::trait_as<monoid>(x);
                        return self | (std::vector{anyxx::trait_as<monoid>(
                                          r)});  // NOLINT
                      }),
     ANY_OP_DEFAULTED(anyxx::self, |, concat,
                      ((anyxx::any_forward_range<anyxx::self, anyxx::self,
                                                 anyxx::const_observer> const&)),
                      const,
                      [&x](const auto& r) {
                        auto self = anyxx::trait_as<monoid>(x);
                        return std::ranges::fold_left(
                            r | std::views::transform([](auto y) {
                              return anyxx::trait_as<monoid>(y);
                            }),
                            self, [&](auto const& m1, auto const& m2) {
                              return m1 + m2;
                            });
                      }),
     ANY_OP_DEFAULTED(bool, ==, equal, (anyxx::self const&), const,
                      ([&x](auto const& r) { return x == r; }))),
    , )

}  // namespace example_2b

ANY_MODEL_MAP((int), example_2b::monoid) {
  static monoid_trait<int> concat(int self, auto const& r) {
    return monoid_trait<int>{std::ranges::fold_left(
        r, self, [&](int m1, int m2) { return m1 + m2; })};
  };
};

ANY_MODEL_MAP((std::string), example_2b::monoid) {
  static monoid_trait<std::string> op(std::string const& self,
                                      std::string const& r) {
    return self + r;
  };
};

namespace example_2b {

template <typename M>
void test_monoid(monoid_trait<M> const& m,
                 anyxx::any_forward_range<monoid_trait<M>, monoid_trait<M>,
                                          anyxx::const_observer>
                     r) {
  using type_1 = decltype(m + (monoid_trait<M>{}) + m);
  using type_2 = decltype(m + (m + (monoid_trait<M>{})));
  static_assert(std::same_as<type_1, type_2>);
  static_assert(std::same_as<type_1, monoid_trait<M>>);
  auto c1 = m + monoid_trait<M>{} + m == m + m + monoid_trait<M>{};
  CHECK(c1);
  auto c2 = (m | r) == std::ranges::fold_left(
                           r, m,
                           [&](monoid_trait<M> const& m1,
                               [[maybe_unused]] monoid_trait<M> const& m2) {
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
  auto vi = std::vector<monoid_trait<int>>{{2}, {3}};
  anyxx::any_forward_range<monoid_trait<int>, monoid_trait<int>, anyxx::value>
      ri = vi;
  static_assert(std::same_as<x_t::any_t, monoid_trait<int>>);
  static_assert(is_any_self_forward_range<anyxx::any_forward_range<
                    anyxx::self, anyxx::self, anyxx::const_observer> const&>);
  std::println(
      "{}",
      typeid(
          anyxx::jacket_param<x_t::any_t,
                              anyxx::any_forward_range<anyxx::self, anyxx::self,
                                                       anyxx::const_observer> const&>)
          .name());
  static_assert(
      std::same_as<
          std::decay_t<anyxx::jacket_param<
              x_t::any_t, anyxx::any_forward_range<anyxx::self, anyxx::self,
                                                   anyxx::const_observer> const&>>,
          anyxx::any_forward_range<monoid_trait<int>, monoid_trait<int>,
                                   anyxx::const_observer>>);
  x | ri;
  test_monoid<int>(1, std::vector<monoid_trait<int>>{{2}, {3}});
  test_monoid<std::string>(
      "1"s, std::vector<monoid_trait<std::string>>{{"2"s}, {"3"s}});
}

#endif