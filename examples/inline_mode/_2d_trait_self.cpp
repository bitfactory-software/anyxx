#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

//namespace example_2d {
//
//ANY(any_has_equal,
//    (ANY_METHOD_DEFAULTED(bool, is_equal, (anyxx::self const), const,
//                          [&x](T const& y) { return x == y; })),
//    , )
//
//}  // namespace example_2d
//
//TEST_CASE("example 2da any_has_equal") {
//  using namespace example_2d;
//  using namespace anyxx;
//  using namespace std::string_literals;
//
//  any_has_equal a{std::in_place, "a"s};
//  any_has_equal b{std::in_place, "b"s};
//  CHECK(a.is_equal(a));
//  CHECK(!b.is_equal(a));
//}

