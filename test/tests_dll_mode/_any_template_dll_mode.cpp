#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <map>  // NOLINT
#include <string>

#include "test/component_base/templated_anys.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 5046)
#endif

static_assert(anyxx::is_in_dll_mode);

namespace {

template <typename KEY, typename VALUE>
void test_any_map_template(
    anyxx::any_<anyxx::cref, test::component_base::map, KEY, VALUE> map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}
}  // namespace

struct test::component_base::X {
  std::string s_;
  [[nodiscard]] std::string to_string() const { return s_; }
};

namespace template_test {

TRAIT(stringable, anyxx::dynamic_copyable,
      (FN(std::string, to_string, (), const)))

// using any_to_string_const_observer_dyns =
//     any_to_string<anyxx::cref>;

TRAIT_TEMPLATE(((KEY)), map_to_string, anyxx::dynamic_copyable, (),
               (FN((anyxx::any_<anyxx::cref, stringable>), at, (KEY const&),
                   const)))

MODEL_MAP(int, stringable) {
  auto to_string(int const& x) -> std::string { return std::to_string(x); };
};
MODEL_MAP(double, stringable) {
  auto to_string(double const& x) -> std::string { return std::to_string(x); };
};

using namespace test::component_base;
using namespace anyxx;

}  // namespace template_test

using namespace template_test;

ANY_META_CLASS_STATIC(int)
ANY_META_CLASS_STATIC(double)
ANY_META_CLASS_STATIC(
    std::map<int, std::map<std::string, std::map<int, double>>>)
ANY_META_CLASS_STATIC(std::map<std::string, std::map<int, double>>)

anyxx::register_trait<std::map<std::string, int>, template_test::map,
                      std::string, int>
    __;

TEMPLATE_MODEL_MAP((std::map<std::string, int>), template_test::map,
                   ((std::string), (int))) {
  int const& at(std::map<std::string, int> const& x, std::string const& i) {
    return x.at(i);
  };
};

using KEY1 = anyxx::any_<anyxx::cref, recursive_map, std::string,
                         anyxx::any_<anyxx::cref, mutable_map, int, double>,
                         anyxx::cref>;
anyxx::register_trait<
    std::map<int, std::map<std::string, std::map<int, double>>>, recursive_map,
    int, KEY1>
    __;

using KEY2 =
    anyxx::any_<anyxx::cref, recursive_map, std::string,
                anyxx::any_<anyxx::cref, map, int, double>, anyxx::cref>;
anyxx::register_trait<
    std::map<int, std::map<std::string, std::map<int, double>>>, recursive_map,
    int, KEY2>
    __;

using KEY3 = anyxx::any_<anyxx::mutref, mutable_recursive_map, std::string,
                         anyxx::any_<anyxx::mutref, mutable_map, int, double>,
                         anyxx::mutref>;
anyxx::register_trait<
    std::map<int, std::map<std::string, std::map<int, double>>>,
    mutable_recursive_map, int, KEY3>
    __;

using KEY4 = anyxx::any_<anyxx::cref, map, int, double>;
anyxx::register_trait<std::map<std::string, std::map<int, double>>,
                      test::component_base::recursive_map, std::string, KEY4>
    __;

using KEY5 = anyxx::any_<anyxx::cref, mutable_map, int, double>;
anyxx::register_trait<std::map<std::string, std::map<int, double>>,
                      test::component_base::recursive_map, std::string, KEY5>
    __;

using KEY6 = anyxx::any_<anyxx::mutref, mutable_map, int, double>;
anyxx::register_trait<std::map<std::string, std::map<int, double>>,
                      test::component_base::mutable_recursive_map, std::string,
                      KEY6>
    __;

TEST_CASE("any_ template test") {
  using namespace template_test;
  using namespace anyxx;
  using namespace test::component_base;

  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_any_map_lambda = [](any_<cref, map, std::string, int> map_i) {
    REQUIRE(map_i.size() == 2);
    REQUIRE(map_i.at("one") == 1);
    REQUIRE(map_i.at("two") == 2);
  };
  test_any_map_lambda(map_string_to_int);

  test_any_map_template<std::string, int>(map_string_to_int);

  auto test_any_map_to_string_lambda =
      [](anyxx::any_<anyxx::cref, map_to_string, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1");
        REQUIRE(map_i.at("two").to_string() == "2");
      };
  test_any_map_to_string_lambda(map_string_to_int);
}

TEST_CASE("any_ template test2") {
  using namespace template_test;
  using namespace anyxx;
  using namespace test::component_base;

  std::map<std::string, double> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_any_map_to_string_lambda =
      [](anyxx::any_<anyxx::cref, map_to_string, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1.000000");
        REQUIRE(map_i.at("two").to_string() == "2.000000");
      };
  test_any_map_to_string_lambda(map_string_to_int);
}

TEST_CASE("any_ template test3") {
  using namespace template_test;
  using namespace anyxx;
  using namespace test::component_base;

  std::map<int, std::map<std::string, std::map<int, double>>> m = {
      {1, {{"one", {{1, 3.14}, {2, 6.28}}}, {"two", {{3, 3.333}}}}},
      {2, {{"one", {{4, 4.14}, {5, 4.28}}}, {"two", {{6, 4.333}}}}}};

  auto test_map_lambda =
      [](anyxx::any_<
          anyxx::cref, recursive_map, int,
          anyxx::any_<anyxx::cref, recursive_map, std::string,
                      anyxx::any_<anyxx::cref, map, int, double>, anyxx::cref>,
          anyxx::cref>
             map_i) {
        auto x = map_i.at(1);
        auto y = x.at("one");
        auto z = y.at(1);
        REQUIRE(z == 3.14);
        REQUIRE(map_i.at(1).at("one").at(1) == 3.14);
        REQUIRE(map_i.at(2).at("one").at(4) == 4.14);
      };
  test_map_lambda(m);

  auto test_map_lambda_mutate =
      [](anyxx::any_<
          anyxx::mutref, mutable_recursive_map, int,
          anyxx::any_<anyxx::mutref, mutable_recursive_map, std::string,
                      anyxx::any_<anyxx::mutref, mutable_map, int, double>>>
             map_i) {
        auto x = map_i.at(1);
        auto y = x.at("one");
        auto z = y.at(1);
        REQUIRE(z == 3.14);
        REQUIRE(map_i.at(1).at("one").at(1) == 3.14);
        REQUIRE(map_i.at(2).at("one").at(4) == 4.14);
        map_i.at(2).at("one").at(4) = 8.28;
      };
  test_map_lambda_mutate(m);
  REQUIRE(m[2]["one"][4] == 8.28);
}

TEST_CASE("any_ template test4") {
  using namespace template_test;
  using namespace anyxx;
  using namespace test::component_base;

  std::map<int, double> m = {{3, 3.333}, {6, 4.333}};

  anyxx::any_<anyxx::cref, map, int, double> any_mutable_map{m};
  REQUIRE(any_mutable_map.at(3) == 3.333);
}
