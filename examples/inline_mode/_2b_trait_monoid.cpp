#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_std.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <string>

#ifdef __cpp_lib_ranges_fold

// count arguments
#define ANYXX_NARGS(...) ANYXX_NARGS_(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define ANYXX_NARGS_(_10, _9, _8, _7, _6, _5, _4, _3, _2, _1, N, ...) N

// utility (concatenation)
#define ANYXX_CAT(a, ...) ANYXX_PRIMITIVE_CAT(a, __VA_ARGS__)
#define ANYXX_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define ANYXX_GET_ELEM(N, ...) ANYXX_CAT(ANYXX_GET_ELEM_, N)(__VA_ARGS__)
#define ANYXX_GET_ELEM_0(_0, ...) _0
#define ANYXX_GET_ELEM_1(_0, _1, ...) _1
#define ANYXX_GET_ELEM_2(_0, _1, _2, ...) _2
#define ANYXX_GET_ELEM_3(_0, _1, _2, _3, ...) _3
#define ANYXX_GET_ELEM_4(_0, _1, _2, _3, _4, ...) _4
#define ANYXX_GET_ELEM_5(_0, _1, _2, _3, _4, _5, ...) _5
#define ANYXX_GET_ELEM_6(_0, _1, _2, _3, _4, _5, _6, ...) _6
#define ANYXX_GET_ELEM_7(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7
#define ANYXX_GET_ELEM_8(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define ANYXX_GET_ELEM_9(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, ...) _9
#define ANYXX_GET_ELEM_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) _10

// Get last argument - placeholder decrements by one
#define ANYXX_GET_LAST(...) ANYXX_GET_ELEM(ANYXX_NARGS(__VA_ARGS__), _, __VA_ARGS__ ,,,,,,,,,,,)
// usage: 
//ANYXX_GET_LAST((A), B)
// expands to:
//  B


#define ANYXX_COMPL(b) ANYXX_PRIMITIVE_CAT(ANYXX_COMPL_, b)
#define ANYXX_COMPL_0 1
#define ANYXX_COMPL_1 0

#define ANYXX_BITAND(x) ANYXX_PRIMITIVE_CAT(ANYXX_BITAND_, x)
#define ANYXX_BITAND_0(y) 0
#define ANYXX_BITAND_1(y) y

#define ANYXX_CHECK_N(x, n, ...) n
#define ANYXX_CHECK(...) ANYXX_CHECK_N(__VA_ARGS__, 0,)
#define ANYXX_PROBE(x) x, 1,

#define ANYXX_IS_PAREN(x) ANYXX_CHECK(ANYXX_IS_PAREN_PROBE x)
#define ANYXX_IS_PAREN_PROBE(...) ANYXX_PROBE(~)

#define ANYXX_NOT(x) ANYXX_CHECK(ANYXX_PRIMITIVE_CAT(ANYXX_NOT_, x))
#define ANYXX_NOT_0 ANYXX_PROBE(~)

#define ANYXX_BOOL(x) ANYXX_COMPL(ANYXX_NOT(x))

#define ANYXX_IIF(c) ANYXX_PRIMITIVE_CAT(ANYXX_IIF_, c)
#define ANYXX_IIF_0(t, ...) __VA_ARGS__
#define ANYXX_IIF_1(t, ...) t

#define ANYXX_IF(c) ANYXX_IIF(ANYXX_BOOL(c))

#define ANYXX_EAT(...)
#define ANYXX_EXPAND(...) __VA_ARGS__
#define ANYXX_WHEN(c) ANYXX_IF(c)(ANYXX_EXPAND, EAT)

#define ANYXX_PRIMITIVE_COMPARE(x, y) ANYXX_IS_PAREN(ANYXX_COMPARE_ ## x ( ANYXX_COMPARE_ ## y) (()))

#define ANYXX_IS_COMPARABLE(x) ANYXX_IS_PAREN( ANYXX_CAT(ANYXX_COMPARE_, x) (()) )

#define ANYXX_NOT_EQUAL(x, y) \
ANYXX_IIF(ANYXX_BITAND(ANYXX_IS_COMPARABLE(x))(ANYXX_IS_COMPARABLE(y)) )(ANYXX_PRIMITIVE_COMPARE, 1 ANYXX_EAT)(x, y)

#define ANYXX_EQUAL(x, y) ANYXX_COMPL(ANYXX_NOT_EQUAL(x, y))

#define ANYXX_COMPARE_auto(x) x

#define ANYXX_JACKET_PARAM_TYPE(...) \
ANYXX_IF(ANYXX_EQUAL(ANYXX_GET_LAST(__VA_ARGS__), auto))( auto, anyxx::jacket_param<ANYXX_UNPAREN(ANYXX_GET_ELEM_0(__VA_ARGS__))>)   
//usage:
//ANYXX_JACKET_PARAM_TYPE((std::vector<int> const&), auto) -> auto
//ANYXX_JACKET_PARAM_TYPE((std::vector<int> const&)) -> anyxx::jacket_param<std::vector<int> const&>



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
      anyxx::any_forward_range<Any, Any,
                               typename AnyForwardRange::erased_data_t,
                               typename AnyForwardRange::dispatch_t> const&;
};
//
template <typename AnyConstObserver, typename AnyMutableObserver,
          typename AnyValue, typename AnyForwardRange>
  requires is_any_self_forward_range<AnyForwardRange>
struct translate_v_table_param<AnyConstObserver, AnyMutableObserver, AnyValue,
                               AnyForwardRange const&> {
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
