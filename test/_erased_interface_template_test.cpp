#include <cmath>
#include <iostream>
#include <map>
#include <string>

#include "virtual_void/data/has_no_meta/value.h"
#include "virtual_void/data/has_type_info/observer.h"
#include "virtual_void/interface/declare_macro.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace ::virtual_void;

struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

namespace {
    ERASED_INTERFACE(to_string_i,
                          (INTERFACE_CONST_METHOD(std::string, to_string)))

    ERASED_INTERFACE_TEMPLATE(((KEY), (VALUE)), map_t_i,
                          (INTERFACE_CONST_METHOD(VALUE const&, at, KEY),
                           INTERFACE_CONST_METHOD(std::size_t, size)))

    ERASED_INTERFACE_TEMPLATE(((KEY)), map_s_t_i,
                          (INTERFACE_CONST_METHOD(to_string_i<data::has_no_meta::value>, at, KEY)))

    template<>
    struct to_string_i_v_table_map<int> : to_string_i_default_v_table_map<int> {
      auto to_string(int const* x) -> std::string { return std::to_string(*x); };
    };
    template<>
    struct to_string_i_v_table_map<double> : to_string_i_default_v_table_map<double> {
      auto to_string(double const* x) -> std::string { return std::to_string(*x); };
    };
}  // namespace

template <typename KEY, typename VALUE>
void test_map_t_i_template(
    map_t_i<data::has_type_info::const_observer, KEY, VALUE> map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}

TEST_CASE("interface template test") {
  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_map_t_i_lambda =
      [](map_t_i<data::has_type_info::const_observer, std::string, int> map_i) {
        REQUIRE(map_i.size() == 2);
        REQUIRE(map_i.at("one") == 1);
        REQUIRE(map_i.at("two") == 2);
      };
  test_map_t_i_lambda(map_string_to_int);

  test_map_t_i_template<std::string, int>(map_string_to_int);

  auto test_map_s_t_i_lambda =
      [](map_s_t_i<data::has_type_info::const_observer, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1");
        REQUIRE(map_i.at("two").to_string() == "2");
      };
  test_map_s_t_i_lambda(map_string_to_int);
}

TEST_CASE("interface template test2") {
  std::map<std::string, double> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_map_s_t_i_lambda =
      [](map_s_t_i<data::has_type_info::const_observer, std::string> map_i) {
        REQUIRE(map_i.at("one").to_string() == "1.000000");
        REQUIRE(map_i.at("two").to_string() == "2.000000");
      };
  test_map_s_t_i_lambda(map_string_to_int);
}