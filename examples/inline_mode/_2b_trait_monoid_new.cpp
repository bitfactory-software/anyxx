#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_range.hpp>
#include <catch2/catch_test_macros.hpp>
#include <list>
#include <print>
#include <string>
#include <vector>

#ifdef __cpp_lib_ranges_fold

namespace example_monoid {

TRAIT_EX(semigroup,
         (ANY_FN_PURE(anyxx::self, op, (anyxx::self const&), const),
          ANY_FN_DEF(bool, equal_to, (anyxx::self const&), const,
                     ([&x](auto const& r) { return x == r; }))),
         ,
         (template <typename Proxy> friend bool operator==(
             anyxx::any<Proxy, semigroup> const& l,
             anyxx::any<Proxy, semigroup> const& r) { return l.equal_to(r); }))

TRAIT_EX_(
    monoid, semigroup,
    (ANY_FN_PURE(anyxx::self, op, (anyxx::self const&), const)),
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
    (template <typename Proxy> friend bool operator==(
        anyxx::any<Proxy, monoid> const& l,
        anyxx::any<Proxy, monoid> const& r) { return l.equal_to(r); }))

template <typename V>
struct plus_mononid_model_map : monoid_default_model_map<V> {
  static auto op(V self, V r) {
    using namespace anyxx;
    std::println("op {}", typeid(V).name());
    return self + r;
  };
};

}  // namespace example_monoid

template <>
struct example_monoid::monoid_model_map<int>
    : example_monoid::plus_mononid_model_map<int> {
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
struct example_monoid::monoid_model_map<std::string>
    : example_monoid::plus_mononid_model_map<std::string> {
  static auto identity() {
    std::println("identy {}", typeid(std::string).name());
    return std::string{};
  };
};

namespace example_monoid {

template <anyxx::is_any Monoid>
void test_monoid_traited(
    Monoid const& m, anyxx::any_forward_range<Monoid, Monoid, anyxx::cref>& r);
template <typename P1>
  requires(!anyxx::is_any<P1>)
void test_monoid(P1 const& p1, std::ranges::forward_range auto const& r) {
  using any_monoid = typename anyxx::using_<P1>::template as<monoid>;
  test_monoid_traited<any_monoid>(any_monoid{p1}, r);
}
template <anyxx::is_any Monoid>
void test_monoid(
    Monoid const& m,
    anyxx::any_forward_range<Monoid, Monoid, anyxx::cref> const& r) {
  test_monoid_traited<Monoid>(m, r);
}
template <anyxx::is_any Monoid>
void test_monoid_traited(
    Monoid const& m,
    anyxx::any_forward_range<Monoid, Monoid, anyxx::cref> const& r) {
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

}  // namespace example_monoid
//
TEST_CASE("example_monoid simple") {
  using namespace anyxx;
  using namespace example_monoid;
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

TEST_CASE("example_monoid monoid int") {
  using namespace example_monoid;
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
// TEST_CASE("example 2b monoid b") {
//   using namespace example_monoid;
//   using namespace std::string_literals;
//   using namespace anyxx;
//
//   test_monoid("1"s, std::vector{"2"s, "3"s});
//   test_monoid<using_<std::string>::as<monoid>>(
//       trait_as<monoid>("1"s),
//       std::vector<using_<std::string>::as<monoid>>{{"2"s}, {"3"s}});
// }
// TEST_CASE("example 2b monoid c") {
//   using namespace example_monoid;
//   using namespace std::string_literals;
//   using namespace anyxx;
//
//   test_monoid<any_monoid<anyxx::val>>("1"s, make_a_range(true));
// }  // NOLINT
// TEST_CASE("example 2b monoid d") {
//   using namespace example_monoid;
//   using namespace std::string_literals;
//   using namespace anyxx;
//
//   test_monoid<any_monoid<anyxx::val>>("1"s, make_a_range(false));
// }
//
// TEST_CASE("static 1") {
//   using namespace example_monoid;
//   using namespace std::string_literals;
//   using namespace anyxx;
//
//   using_<int>::as<TestTrait> t{0};
//   CHECK(t.id() == 0);
// }
//
// TEST_CASE("static 2") {
//   using namespace example_monoid;
//   using namespace std::string_literals;
//   using namespace anyxx;
//
//   using_<int>::as<monoid> im{999};
//   CHECK(im.identity() == 0);
//   CHECK(im.concat(std::vector{trait_as<monoid>(1), trait_as<monoid>(2)}) ==
//   3);
//
//   using_<std::string>::as<monoid> sm{"XXX"};
//   CHECK(sm.identity() == trait_as<monoid>(""s));
//   CHECK(
//       sm.concat(std::vector{trait_as<monoid>("A"s), trait_as<monoid>("B"s)})
//       == trait_as<monoid>("AB"s));
// }

#endif
