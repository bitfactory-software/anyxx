#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace lib_2f {

TRAIT_EX(partial_equality,
         (ANY_OP_DEF(protected, bool, ==, eq, (anyxx::self const&), const,
                     [&x](auto const& r) {
                       return !(trait_as<partial_equality>(x) !=
                                trait_as<partial_equality>(r));
                     }),
          ANY_OP_DEF(public, bool, !=, ne, (anyxx::self const&), const,
                     [&x](auto const& r) {
                       return !(trait_as<partial_equality>(x) ==
                                trait_as<partial_equality>(r));
                     })),
         , , ())

template <typename T>
  requires requires(T const& a, T const& b) {
    { a == b } -> std::convertible_to<bool>;
  }
struct partial_equality_model_map<T> : partial_equality_default_model_map<T> {
  static auto eq(T const& self, T const& r) { return self == r; }
};
static_assert(is_partial_equality_model<int>);
static_assert(is_partial_equality_model<double>);
static_assert(is_partial_equality_model<bool>);
static_assert(is_partial_equality_model<std::string>);

template <anyxx::is_proxy Proxy>
void test_partial_equality_(anyxx::any<Proxy, partial_equality> const& a,
                            anyxx::any<Proxy, partial_equality> const& b) {
  CHECK((a == b) == (b == a));
  CHECK((a != b) == (b != a));
}
template <is_partial_equality_model T>
void test_partial_equality(T const& a, T const& b) {
  using namespace anyxx;
  test_partial_equality_<anyxx::using_<T>>(a, b);
}

}  // namespace lib_2f

namespace app_2f {

struct a_type {
  std::string name;
};

}  // namespace app_2f

static_assert(!lib_2f::is_partial_equality_model<app_2f::a_type>);

namespace app_2f {

struct b_type {
  std::string name;
};

}  // namespace app_2f

ANY_MODEL_MAP((app_2f::b_type), lib_2f::partial_equality) {
  using default_map::eq;
  static auto ne(app_2f::b_type self, app_2f::b_type r) {
    return self.name != r.name;
  }
};
static_assert(lib_2f::is_partial_equality_model<app_2f::b_type>);

TEST_CASE("partial_equality static") {
  lib_2f::test_partial_equality(1, 1);
  lib_2f::test_partial_equality(app_2f::b_type{"A"}, app_2f::b_type{"B"});
}

TEST_CASE("partial_equality dynamic") {
  using any_partial_equality = anyxx::any<anyxx::val, lib_2f::partial_equality>;
  std::vector<std::pair<any_partial_equality, any_partial_equality>> v{
      {app_2f::b_type{"A"}, app_2f::b_type{"A"}},
      {app_2f::b_type{"A"}, app_2f::b_type{"B"}},
      {3, 3},
      {3, 4},
      {3.14, 3.14},
      {3.14, 2.71},
      {"hello", "hello"},
      {"hello", "world"},
  };
  for (auto const& [a, b] : v) {
    lib_2f::test_partial_equality_(a, b);
  }
}
