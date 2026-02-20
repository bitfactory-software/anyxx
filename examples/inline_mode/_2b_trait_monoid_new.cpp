#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_range.hpp>
#include <catch2/catch_test_macros.hpp>
#include <list>
#include <print>
#include <string>
#include <vector>

#ifdef __cpp_lib_ranges_fold

namespace algebra {

TRAIT_EX(semigroup,
         (ANY_FN_PURE(anyxx::self, op, (anyxx::self const&), const),
          ANY_OP_DEF(bool, ==, eq, (anyxx::self const&), const,
                     ([&x](auto const& r) { return x == r; }))),
         , ())

template <typename V>
struct semigroup_plus_model_map : semigroup_default_model_map<V> {
  static auto op(V self, V r) {
    using namespace anyxx;
    std::println("op {}", typeid(V).name());
    return self + r;
  };
};

TRAIT_EX_(
    monoid, semigroup, ,
    (ANY_FN_STATIC_DEF((), anyxx::self, identity, (),
                       []() {
                         using monoid_t =
                             typename anyxx::using_<T>::template as<monoid>;
                         return monoid_t{T{}}.concat(
                             std::ranges::empty_view<monoid_t>{});
                       }),
     ANY_FN_STATIC_DEF((), anyxx::self, concat,
                       ((anyxx::any_forward_range<anyxx::self, anyxx::self,
                                                  anyxx::cref> const&)),
                       [](const auto& r) {
                         using monoid_t =
                             typename anyxx::using_<T>::template as<monoid>;
                         auto id = monoid_t{T{}}.identity();
                         return std::ranges::fold_left(
                             r, id,
                             [&](monoid_t const& m1, monoid_t const& m2) {
                               return m1.op(m2);
                             });
                       })),
    ())

TRAIT_EX_(group, monoid, (ANY_FN_PURE(anyxx::self, inverse, (), const)), , ())
}  // namespace algebra

template <>
struct algebra::semigroup_model_map<int> : semigroup_plus_model_map<int> {};
template <>
struct algebra::monoid_model_map<int> : algebra::monoid_default_model_map<int> {
  static auto concat(auto const& r) {
    using namespace anyxx;
    std::println("concat {}", typeid(int).name());
    return std::ranges::fold_left(
        r, trait_as<monoid>(0),
        [&](using_<int>::as<monoid> const& m1,
            using_<int>::as<monoid> const& m2) { return m1.op(m2); });
  };
};
template <>
struct algebra::group_model_map<int> : algebra::group_default_model_map<int>,
                                       algebra::monoid_model_map<int> {
  static auto inverse(int self) {
    using namespace anyxx;
    return -self;
  };
};

template <>
struct algebra::semigroup_model_map<std::string>
    : semigroup_plus_model_map<std::string> {};
template <>
struct algebra::monoid_model_map<std::string>
    : algebra::monoid_default_model_map<std::string> {
  static auto identity() {
    std::println("identy {}", typeid(std::string).name());
    return std::string{};
  };
};

namespace algebra_test {
using namespace algebra;

template <anyxx::is_any Monoid>
void test_monoid_traited(Monoid const& m,
                         std::ranges::forward_range auto const& r);
template <typename P1>
  requires(!anyxx::is_any<P1>)
void test_monoid(P1 const& p1, std::ranges::forward_range auto const& r) {
  using any_monoid = typename anyxx::using_<P1>::template as<monoid>;
  test_monoid_traited<any_monoid>(any_monoid{p1}, r);
}
template <anyxx::is_any Monoid>
void test_monoid(Monoid const& m, std::ranges::forward_range auto const& r) {
  test_monoid_traited<Monoid>(m, r);
}
template <anyxx::is_any Monoid>
void test_monoid_traited(Monoid const& m,
                         std::ranges::forward_range auto const& r) {
  auto id = m.identity();
  using type_1 = decltype(m.op(id.op(m)));
  using type_2 = decltype(m.op(m.op(id)));
  std::println("type_1: {}, type_2: {}", typeid(type_1).name(),
               typeid(type_2).name());
  static_assert(std::same_as<type_1, type_2>);
  static_assert(std::same_as<type_1, Monoid>);
  static_assert(std::same_as<type_2, Monoid>);
  auto c1 = m.op(id).op(m) == m.op(m).op(id);
  CHECK(c1);
  auto c2 = m.concat(r) ==
            std::ranges::fold_left(
                r, m.identity(),
                [&](Monoid const& m1, [[maybe_unused]] Monoid const& m2) {
                  return m1.op(m2);
                });
  CHECK(c2);
}

// struct not_mappepd{ int v; };

}  // namespace algebra_test
//
TEST_CASE("algebra basics") {
  using namespace anyxx;
  using namespace algebra;
  using namespace std::string_literals;

  using_<int>::as<monoid> x{2};
  using_<int>::as<monoid> y{x};
  using_<int>::as<monoid> z = y;
  CHECK(static_cast<int>(z) == 2);
  using_<int>::as<monoid> a{std::move(x)};
  CHECK(static_cast<int>(a) == 2);
  static_assert(anyxx::is_proxy<decltype(x)::proxy_t>);
  static_assert(anyxx::is_any<decltype(x)>);
  static_assert(
      anyxx::moveable_from<decltype(x)::proxy_t, decltype(y)::proxy_t>);
  static_assert(
      !anyxx::borrowable_from<decltype(x)::proxy_t, decltype(y)::proxy_t>);

  {
    using_<int>::as<monoid> im{999};
    CHECK(im.identity() == trait_as<monoid>(0));
    auto x1 = im.concat(std::vector{trait_as<monoid>(1), trait_as<monoid>(2)});
    auto y2 = trait_as<monoid>(3);
    CHECK(x1 == y2);
  }
  {
    using_<std::string>::as<monoid> sm{"XXX"};
    auto i = sm.identity();

    CHECK(sm.identity() == trait_as<monoid>(""s));
    auto x1 =
        sm.concat(std::vector{trait_as<monoid>("A"s), trait_as<monoid>("B"s)});
    auto y2 = trait_as<monoid>("AB"s);
    CHECK(x1 == y2);
  }
}

TEST_CASE("algebra monoid") {
  using namespace algebra;
  using namespace algebra_test;
  using namespace std::string_literals;
  using namespace anyxx;

  test_monoid((1), std::vector{2, 3});
  test_monoid<any<using_<int>, monoid>>(
      trait_as<monoid>(1), std::vector<any<using_<int>, monoid>>{{2}, {3}});

  test_monoid(("1"s), std::vector{"2"s, "3"s});
  test_monoid<any<using_<int>, monoid>>(
      trait_as<monoid>(1), std::vector<any<using_<int>, monoid>>{{2}, {3}});

  //  test_monoid(not_mappepd{1}, std::vector{not_mappepd{2}, not_mappepd{3}});
}

namespace algebra_test {
using namespace algebra;

template <anyxx::is_any Group>
void test_group_traited(Group const& m,
                        std::ranges::forward_range auto const& r);
template <typename P1>
  requires(!anyxx::is_any<P1>)
void test_group(P1 const& p1, std::ranges::forward_range auto const& r) {
  using any_group = typename anyxx::using_<P1>::template as<group>;
  test_group_traited<any_group>(any_group{p1}, r);
}
template <anyxx::is_any Group>
void test_group(Group const& m, std::ranges::forward_range auto const& r) {
  test_group_traited<Group>(m, r);
}
template <anyxx::is_any Group>
void test_group_traited(Group const& g,
                        [[maybe_unused]]std::ranges::forward_range auto const& r) {
  auto id = g.identity();
  static_assert(std::same_as<decltype(id), Group>);
  auto inv = g.inverse();
  static_assert(std::same_as<decltype(inv), Group>);
  CHECK(g.op(inv) == id);
  using type_1 = decltype(g.op(id.op(g)));
  using type_2 = decltype(g.op(g.op(id)));
  std::println("type_1: {}, type_2: {}", typeid(type_1).name(),
               typeid(type_2).name());
  static_assert(std::same_as<type_1, type_2>);
  static_assert(std::same_as<type_1, Group>);
  static_assert(std::same_as<type_2, Group>);
  auto c1 = g.op(id).op(g) == g.op(g).op(id);
  CHECK(c1);
  //auto c2 = g.concat(r) ==
  //          std::ranges::fold_left(
  //              r, g.identity(),
  //              [&](Group const& g1, [[maybe_unused]] Group const& g2) {
  //                return g1.op(g2);
  //              });
  //CHECK(c2);

  //test_monoid_traited(g, r);
}
}  // namespace algebra_test

TEST_CASE("algebra group") {
  using namespace algebra;
  using namespace anyxx;
  using namespace algebra_test;

  test_group((1), std::vector{2, 3});
  test_group<any<using_<int>, group>>(
      trait_as<group>(1), std::vector<any<using_<int>, group>>{{2}, {3}});
}

#endif
