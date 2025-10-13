#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <anypp/observer.hpp>
#include <anypp/shared_const.hpp>
#include <anypp/unique.hpp>
#include <anypp/any_base.hpp>
#include <anypp/any_meta_class.hpp>

using namespace Catch::Matchers;

using namespace anypp;
using namespace anypp;

struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

namespace {
ANY(to_string_i, (ANY_CONST_METHOD(std::string, to_string)))

ANY_TEMPLATE(((KEY), (VALUE)), map_t_i,
                      (ANY_CONST_METHOD(VALUE const&, at, KEY),
                       ANY_CONST_METHOD(std::size_t, size)))

ANY_TEMPLATE(((KEY), (VALUE)), map_mutable_t_i,
                      (ANY_METHOD(VALUE&, at, KEY),
                       ANY_CONST_METHOD(std::size_t, size)))

ANY_TEMPLATE(((KEY), (VALUE)), map_const_recursive_t_i,
                      (ANY_CONST_METHOD(VALUE, at, KEY),
                       ANY_CONST_METHOD(std::size_t, size)))

ANY_TEMPLATE(((KEY), (VALUE)), map_mutable_recursive_t_i,
                      (ANY_METHOD(VALUE, at, KEY),
                       ANY_CONST_METHOD(std::size_t, size)))

ANY_TEMPLATE(((KEY)), map_to_string_i,
                      (ANY_CONST_METHOD(to_string_i<const_observer>, at, KEY)))

template <>
struct to_string_i_v_table_map<int> : to_string_i_default_v_table_map<int> {
  auto to_string(int const* x) -> std::string { return std::to_string(*x); };
};
template <>
struct to_string_i_v_table_map<double>
    : to_string_i_default_v_table_map<double> {
  auto to_string(double const* x) -> std::string { return std::to_string(*x); };
};
template <>
struct to_string_i_v_table_map<const double> : to_string_i_v_table_map<double> {
};
}  // namespace

template <typename KEY, typename VALUE>
void test_map_t_i_template(map_t_i<const_observer, KEY, VALUE> map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}

TEST_CASE("any template test") {
  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_map_t_i_lambda =
      [](map_t_i<const_observer, std::string, int> map_i) {
        REQUIRE(map_i.size() == 2);
        REQUIRE(map_i.at("one") == 1);
        REQUIRE(map_i.at("two") == 2);
      };
  test_map_t_i_lambda(map_string_to_int);

  test_map_t_i_template<std::string, int>(map_string_to_int);

  auto test_map_to_string_i_lambda =
      [](map_to_string_i<const_observer, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1");
        REQUIRE(map_i.at("two").to_string() == "2");
      };
  test_map_to_string_i_lambda(map_string_to_int);
}

TEST_CASE("any template test2") {
  std::map<std::string, double> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_map_to_string_i_lambda =
      [](map_to_string_i<const_observer, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1.000000");
        REQUIRE(map_i.at("two").to_string() == "2.000000");
      };
  test_map_to_string_i_lambda(map_string_to_int);
}

TEST_CASE("any template test3") {
  std::map<int, std::map<std::string, std::map<int, double>>> map = {
      {1, {{"one", {{1, 3.14}, {2, 6.28}}}, {"two", {{3, 3.333}}}}},
      {2, {{"one", {{4, 4.14}, {5, 4.28}}}, {"two", {{6, 4.333}}}}}};

  auto test_map_lambda =
      [](map_const_recursive_t_i<
          const_observer, int,
          map_const_recursive_t_i<const_observer, std::string,
                                  map_t_i<const_observer, int, double>>>
             map_i) {
        auto x = map_i.at(1);
        auto y = x.at("one");
        auto z = y.at(1);
        REQUIRE(z == 3.14);
        REQUIRE(map_i.at(1).at("one").at(1) == 3.14);
        REQUIRE(map_i.at(2).at("one").at(4) == 4.14);
      };
  test_map_lambda(map);

  auto test_map_lambda_mutate =
      [](map_mutable_recursive_t_i<
          mutable_observer, int,
          map_mutable_recursive_t_i<
              mutable_observer, std::string,
              map_mutable_t_i<mutable_observer, int, double>>>
             map_i) {
        auto x = map_i.at(1);
        auto y = x.at("one");
        auto z = y.at(1);
        REQUIRE(z == 3.14);
        REQUIRE(map_i.at(1).at("one").at(1) == 3.14);
        REQUIRE(map_i.at(2).at("one").at(4) == 4.14);
        map_i.at(2).at("one").at(4) = 8.28;
      };
  test_map_lambda_mutate(map);
  REQUIRE(map[2]["one"][4] == 8.28);
}