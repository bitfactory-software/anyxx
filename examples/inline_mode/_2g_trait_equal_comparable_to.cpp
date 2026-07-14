#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx_range.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace lib_2f {

TRAIT_TEMPLATE(
    ((To)), equal_comparable_to,
    (ANY_OP_MAP_NAMED_FRIEND(
         bool, ==, eq, ((anyxx::use_as_<To, equal_comparable_to, T> const&)),
         const),
     ANY_OP_DEF(public, bool, !=, ne,
                ((anyxx::use_as_<To, equal_comparable_to, T> const&)), const,
                ([&x](auto const& r) {
                  return !(trait_as<equal_comparable_to<To>>(x) ==
                           trait_as<equal_comparable_to<T>>(r));
                }))))

template <typename L, typename R>
using trait_as_equal_comparable_to =
    anyxx::any<equal_comparable_to<R>, anyxx::using_<L>>;

// no memory overhead because EBO and there is no vtable: The
// trait uses static dispatch only.
static_assert(sizeof(trait_as_equal_comparable_to<int, double>) == sizeof(int));
static_assert(is_equal_comparable_to_model<int, int>);
static_assert(is_equal_comparable_to_model<int, double>);
static_assert(is_equal_comparable_to_model<double, double>);

template <typename L, typename R>
  requires is_equal_comparable_to_model<L, R> &&
           is_equal_comparable_to_model<R, L>
void test_equal_comparable_to(trait_as_equal_comparable_to<L, R> const& a,
                              trait_as_equal_comparable_to<R, L> const& b) {
  CHECK((a == b) == (b == a));
  CHECK((a != b) == (b != a));
}
template <typename L, typename R>
  requires is_equal_comparable_to_model<L, R> &&
           is_equal_comparable_to_model<R, L>
void test_equal_comparable_to(L const& a, R const& b) {
  using namespace anyxx;
  test_equal_comparable_to<L, R>(trait_as<equal_comparable_to<R>>(a),
                                 trait_as<equal_comparable_to<L>>(b));
}

template <typename L, typename Range>
  requires is_equal_comparable_to_model<L, std::ranges::range_value_t<Range>> &&
           is_equal_comparable_to_model<std::ranges::range_value_t<Range>, L> &&
           std::ranges::forward_range<Range>
void test_equal_comparable_to_range(L const& a, Range const& r) {
  using namespace anyxx;
  using R = std::ranges::range_value_t<Range>;
  auto a_traited = trait_as<equal_comparable_to<R>>(a);
  auto r_traited = trait_range_as<equal_comparable_to<L>>(r);
  CHECK(*std::ranges::begin(r_traited) == a_traited);
  CHECK(a_traited == *std::ranges::begin(r_traited));
}

}  // namespace lib_2f

namespace app_2f {

struct a_type {
  std::string name_a;
};
struct b_type {
  std::string name_b;
};

bool eq_impl(app_2f::b_type const& b, app_2f::a_type const& a);
}  // namespace app_2f

ANY_TEMPLATE_MODEL_MAP((app_2f::b_type), lib_2f::equal_comparable_to,
                       ((app_2f::a_type))) {
  static bool eq(app_2f::b_type const& self, app_2f::a_type const& r) {
    return eq_impl(self, r);
  };
};
ANY_TEMPLATE_MODEL_MAP((app_2f::a_type), lib_2f::equal_comparable_to,
                       ((app_2f::b_type))) {
  static bool eq(app_2f::a_type const& self, app_2f::b_type const& r) {
    return eq_impl(r, self);
  };
};
namespace app_2f {
inline bool eq_impl(app_2f::b_type const& self, app_2f::a_type const& r) {
  return self.name_b == r.name_a;
};
}  // namespace app_2f

static_assert(
    lib_2f::is_equal_comparable_to_model<app_2f::b_type, app_2f::a_type>);
static_assert(
    lib_2f::is_equal_comparable_to_model<app_2f::a_type, app_2f::b_type>);

static std::vector<int> test_data{
    std::ranges::to<std::vector>(std::ranges::iota_view(0, 1000000))};

bool native_find(double x) {
  return std::ranges::find_if(test_data, [&](int i) { return x == i; }) !=
         test_data.end();
}
bool traited_find(lib_2f::trait_as_equal_comparable_to<double, int> x) {
  return std::ranges::find_if(
             test_data,
             [&](lib_2f::trait_as_equal_comparable_to<int, double> i) {
               return x == i;
             }) != test_data.end();
}

// Now we can use the algorithm defined in the library
// with static dispatch usage:
TEST_CASE("equal_comparable_to static") {
  using namespace anyxx;
  using namespace lib_2f;
  auto a = trait_as<equal_comparable_to<double>>(1);
  auto b = trait_as<equal_comparable_to<int>>(2.0);
  auto x = a == b;  // simple case
  CHECK(x == false);

  lib_2f::test_equal_comparable_to(1, 3.14);
  // this next example shows, why we need concept maps as customization points:
  lib_2f::test_equal_comparable_to(app_2f::a_type{"A"}, app_2f::b_type{"B"});

  lib_2f::test_equal_comparable_to_range(app_2f::a_type{"A"},
                                         std::vector<app_2f::b_type>{{"A"}});
}

TEST_CASE("equal_comparable_to benchmark") {
  using namespace anyxx;
  using namespace lib_2f;

#ifndef _DEBUG
  BENCHMARK("native") { return native_find(1000000 - 1); };
  BENCHMARK("traited") { return traited_find(999999); };
#endif  // !_DEBUG
}
