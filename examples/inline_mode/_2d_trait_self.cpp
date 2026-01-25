#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace example_2d {

// ANY(has_equal,
//     (ANY_FN_DEFAULTED(bool, is_equal, (anyxx::self const&), const,
//                           [&x](T const& y) { return x == y; })), )

TRAIT(has_equal,
      (ANY_FN_DEFAULTED(bool, is_equal, (anyxx::self const&), const,
                            [&x](T const& y) { return x == y; })))

template <typename Box = anyxx::shared_const>
using any_has_equal = anyxx::any<Box, has_equal>;

}  // namespace example_2d

TEST_CASE("example 2da any_has_equal") {
  using namespace example_2d;
  using namespace anyxx;
  using namespace std::string_literals;

  {
    any_has_equal<> a{std::in_place, "a"s};
    any_has_equal<> b{std::in_place, "b"s};
    CHECK(a.is_equal(a));
    CHECK(!b.is_equal(a));
  }
  {
    auto a = trait_as<has_equal>("a"s);
    auto b = trait_as<has_equal>("b"s);
    CHECK(a.is_equal(a));
    CHECK(!b.is_equal(a));
  }
}

namespace example_2d {

TRAIT_(has_plus, has_equal,
      (ANY_FN_DEFAULTED(anyxx::self, plus, (anyxx::self const&), const,
                            ([&x](T const& y) {
                              return anyxx::trait_as<has_plus>(x) +
                                     anyxx::trait_as<has_plus>(y);
                            })),
       ANY_OP_DEFAULTED(anyxx::self, +, plus_op, (anyxx::self const&), const,
                        ([&x](T const& y) { return x + y; }))))

template <typename Box = anyxx::value>
using any_has_plus = anyxx::any<Box, has_plus>;

}  // namespace example_2d

TEST_CASE("example 2db any_has_plus dynamic") {
  using namespace example_2d;
  using namespace anyxx;
  using namespace std::string_literals;
  {
    any_has_plus<> a{std::in_place, "a"s};
    any_has_plus<> b{std::in_place, "b"s};
    CHECK((a + a).is_equal(any_has_plus<>{"aa"s}));
    CHECK((a + b).is_equal(any_has_plus<>{"ab"s}));
    CHECK((a.plus(a)).is_equal(any_has_plus<>{"aa"s}));
    CHECK((a.plus(b)).is_equal(any_has_plus<>{"ab"s}));
  }
}
TEST_CASE("example 2db any_has_plus static") {
  using namespace example_2d;
  using namespace anyxx;
  using namespace std::string_literals;
  {
    auto a = trait_as<has_plus>("a"s);
    auto b = trait_as<has_plus>("b"s);
    auto aa = trait_as<has_plus>("aa"s);
    auto bb = trait_as<has_plus>("ab"s);
    CHECK((a + a).is_equal(aa));
    CHECK((a + b).is_equal(bb));
    CHECK((a.plus(a)).is_equal(aa));
    CHECK((a.plus(b)).is_equal(bb));
  }
}
