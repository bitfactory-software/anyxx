#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;

struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

VV_RUNTIME_STATIC(X)

namespace {
VV_INTERFACE(to_string_i, (VV_CONST_METHOD(std::string, to_string)))

VV_INTERFACE_TEMPLATE(((KEY), (VALUE)), map_t_i,
                      (VV_CONST_METHOD(VALUE const&, at, KEY),
                       VV_CONST_METHOD(std::size_t, size)))

VV_INTERFACE_TEMPLATE(((KEY), (VALUE)), map_mutable_t_i,
                      (VV_METHOD(VALUE&, at, KEY),
                       VV_CONST_METHOD(std::size_t, size)))

VV_INTERFACE_TEMPLATE(((KEY), (VALUE)), map_tt_i,
                      (VV_CONST_METHOD(VALUE, at, KEY),
                       VV_CONST_METHOD(std::size_t, size)))

VV_INTERFACE_TEMPLATE(((KEY), (VALUE)), map_mutable_tt_i,
                      (VV_METHOD(VALUE, at, KEY),
                       VV_CONST_METHOD(std::size_t, size)))

VV_INTERFACE_TEMPLATE(((KEY)), map_s_t_i,
                      (VV_CONST_METHOD(to_string_i<const_observer>, at, KEY)))

template <>
struct to_string_i_v_table_map<int> : to_string_i_default_v_table_map<int> {
  auto to_string(int const* x) -> std::string { return std::to_string(*x); };
};
template <>
struct to_string_i_v_table_map<int const> : to_string_i_v_table_map<int> {};
template <>
struct to_string_i_v_table_map<double>
    : to_string_i_default_v_table_map<double> {
  auto to_string(double const* x) -> std::string { return std::to_string(*x); };
};
template <>
struct to_string_i_v_table_map<const double> : to_string_i_v_table_map<double> {
};
// template <>
// struct to_string_i_v_table_map<std::map<int, std::map<std::string,
// std::map<int, double>>>> {
//   auto to_string(std::map<int, std::map<std::string, std::map<int, double>>>
//   const* x) -> std::string { return "????"; };
// };
template <>
struct to_string_i_v_table_map<std::map<int, std::map<int, double>>> {
  auto to_string(std::map<int, std::map<int, double>> const* x) -> std::string {
    return "????";
  };
};
template <>
struct to_string_i_v_table_map<std::map<std::string, std::map<int, double>>> {
  auto to_string(std::map<std::string, std::map<int, double>> const* x)
      -> std::string {
    return "????";
  };
};
}  // namespace

VV_RUNTIME_STATIC(int)
VV_RUNTIME_STATIC(double)
VV_RUNTIME_STATIC(std::map<int, double>)
VV_RUNTIME_STATIC(std::map<int, std::map<std::string, std::map<int, double>>>)
VV_RUNTIME_STATIC(std::map<std::string, int>)
VV_RUNTIME_STATIC(std::map<std::string, double>)
VV_RUNTIME_STATIC(std::map<std::string, std::map<int, double>>)
VV_RUNTIME_STATIC(std::map<std::string, std::map<std::string, std::map<int, double>>>)

VV_V_TABLE_INSTANCE(, int, to_string_i)
VV_V_TABLE_INSTANCE(, double, to_string_i)
VV_V_TABLE_INSTANCE(, VV_NAME(std::map<std::string, std::map<int, double>>),
                    to_string_i)

VV_V_TABLE_TEMPLATE_INSTANCE(
    , VV_NAME(std::map<int, std::map<std::string, std::map<int, double>>>),
    map_s_t_i, int)

VV_V_TABLE_TEMPLATE_INSTANCE(, VV_NAME(std::map<std::string, double>),
                             map_s_t_i, std::string)

VV_V_TABLE_TEMPLATE_INSTANCE(, VV_NAME(std::map<int, double>), map_mutable_t_i,
                             int, double)
VV_V_TABLE_TEMPLATE_INSTANCE(, VV_NAME(std::map<std::string, int>), map_s_t_i,
                             std::string)

VV_V_TABLE_TEMPLATE_INSTANCE(
    , VV_NAME(std::map<std::string, std::map<int, double>>), map_mutable_tt_i,
    std::string, VV_NAME(map_mutable_t_i<mutable_observer, int, double>))

VV_V_TABLE_TEMPLATE_INSTANCE(
    , VV_NAME(std::map<int, std::map<std::string, std::map<int, double>>>),
    map_mutable_tt_i, int,
    VV_NAME(map_mutable_tt_i<mutable_observer, std::string,
                             map_mutable_t_i<mutable_observer, int, double>>))

VV_V_TABLE_TEMPLATE_INSTANCE(, VV_NAME(std::map<int, double>), map_mutable_tt_i,
                             int, double)

VV_V_TABLE_TEMPLATE_INSTANCE(
    , VV_NAME(std::map<int, std::map<std::string, std::map<int, double>>>),
    map_mutable_tt_i, int,
    VV_NAME(std::map<std::string, std::map<int, double>>))

VV_V_TABLE_TEMPLATE_INSTANCE(
    , VV_NAME(std::map<std::string, std::map<int, double>>), map_mutable_tt_i,
    std::string, VV_NAME(map_mutable_tt_i<mutable_observer, int, double>))

VV_V_TABLE_TEMPLATE_INSTANCE(, VV_NAME(std::map<std::string, int>),
                             map_mutable_tt_i, std::string, int)
VV_V_TABLE_TEMPLATE_INSTANCE(, VV_NAME(std::map<std::string, int>), map_t_i,
                             std::string, int)

VV_V_TABLE_TEMPLATE_INSTANCE(
    , VV_NAME(std::map<int, std::map<std::string, std::map<int, double>>>),
    map_tt_i, int,
    VV_NAME(map_tt_i<const_observer, std::string,
                     map_t_i<const_observer, int, double>>))

VV_V_TABLE_TEMPLATE_INSTANCE(
    ,
    VV_NAME(
        std::map<std::string, std::map<std::string, std::map<int, double>>>),
    map_tt_i, std::string,
    VV_NAME(map_tt_i<const_observer, std::string,
                     map_t_i<const_observer, int, double>>))

VV_V_TABLE_TEMPLATE_INSTANCE(
    , VV_NAME(std::map<std::string, std::map<int, double>>), map_tt_i,
    std::string, VV_NAME(map_t_i<const_observer, int, double>))

VV_V_TABLE_TEMPLATE_INSTANCE(, VV_NAME(std::map<int, double>), map_t_i, int,
                             double)

// VV_V_TABLE_INSTANCE(, x, y)

template <typename KEY, typename VALUE>
void test_map_t_i_template(map_t_i<const_observer, KEY, VALUE> map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}

TEST_CASE("interface template test") {
  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_map_t_i_lambda =
      [](map_t_i<const_observer, std::string, int> map_i) {
        REQUIRE(map_i.size() == 2);
        REQUIRE(map_i.at("one") == 1);
        REQUIRE(map_i.at("two") == 2);
      };
  test_map_t_i_lambda(map_string_to_int);

  test_map_t_i_template<std::string, int>(map_string_to_int);

  auto test_map_s_t_i_lambda =
      [](map_s_t_i<const_observer, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1");
        REQUIRE(map_i.at("two").to_string() == "2");
      };
  test_map_s_t_i_lambda(map_string_to_int);
}

TEST_CASE("interface template test2") {
  std::map<std::string, double> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_map_s_t_i_lambda =
      [](map_s_t_i<const_observer, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1.000000");
        REQUIRE(map_i.at("two").to_string() == "2.000000");
      };
  test_map_s_t_i_lambda(map_string_to_int);
}

TEST_CASE("interface template test3") {
  std::map<int, std::map<std::string, std::map<int, double>>> map = {
      {1, {{"one", {{1, 3.14}, {2, 6.28}}}, {"two", {{3, 3.333}}}}},
      {2, {{"one", {{4, 4.14}, {5, 4.28}}}, {"two", {{6, 4.333}}}}}};

  auto test_map_lambda =
      [](map_tt_i<const_observer, int,
                  map_tt_i<const_observer, std::string,
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
      [](map_mutable_tt_i<
          mutable_observer, int,
          map_mutable_tt_i<mutable_observer, std::string,
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