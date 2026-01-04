#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace example_2d {

ANY(any_has_equal,
    (ANY_METHOD_DEFAULTED(bool, is_equal, (anyxx::self const), const,
                          [&x](T const& y) { return x == y; })),
    , )

}  // namespace example_2d

TEST_CASE("example 2da any_has_equal") {
  using namespace example_2d;
  using namespace anyxx;
  using namespace std::string_literals;

  {
    any_has_equal a{std::in_place, "a"s};
    any_has_equal b{std::in_place, "b"s};
    CHECK(a.is_equal(a));
    CHECK(!b.is_equal(a));
  }
  {
    auto a = trait_as<any_has_equal>("a"s);
    auto b = trait_as<any_has_equal>("b"s);
    CHECK(a.is_equal(a));
    CHECK(!b.is_equal(a));
  }
}

namespace example_2d {

ANY_(any_has_plus, any_has_equal,
     (ANY_METHOD_DEFAULTED(anyxx::self, plus, (anyxx::self const), const,
                           ([&x](T const& y) {
                             return anyxx::trait_as<any_has_plus>(x) +
                                    anyxx::trait_as<any_has_plus>(y);
                           })),
      ANY_OP_DEFAULTED(anyxx::self, +, plus_op, (anyxx::self const), const,
                       ([&x](T const& y) { return x + y; }))),
     anyxx::value, )

}  // namespace example_2d

TEST_CASE("example 2db any_has_plus") {
  using namespace example_2d;
  using namespace anyxx;
  using namespace std::string_literals;

  {
    any_has_plus a{std::in_place, "a"s};
    any_has_plus b{std::in_place, "b"s};
    CHECK((a + a).is_equal(any_has_plus{"aa"s}));
    CHECK((a + b).is_equal(any_has_plus{"ab"s}));
    CHECK((a.plus(a)).is_equal(any_has_plus{"aa"s}));
    CHECK((a.plus(b)).is_equal(any_has_plus{"ab"s}));
  }
  {
    auto a = trait_as<any_has_plus>("a"s);
    auto b = trait_as<any_has_plus>("b"s);
    auto aa = trait_as<any_has_plus>("aa"s);
    auto bb = trait_as<any_has_plus>("ab"s);
    CHECK((a + a).is_equal(aa));
    CHECK((a + b).is_equal(bb));
    CHECK((a.plus(a)).is_equal(aa));
    CHECK((a.plus(b)).is_equal(bb));
  }
}
