#include <cmath>
#include <iostream>
#include <map>
#include <string>

// #include "../include/virtual_void/erased/data/has_no_meta/observer.h"
#include "../include/virtual_void/erased/data/has_type_info/observer.h"
#include "../include/virtual_void/erased/interface/declare_macro.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace ::virtual_void::erased;

struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

namespace {
ERASED_INTERFACE_TEMPLATE(((KEY), (VALUE)), map_t_i,
                          (INTERFACE_CONST_METHOD(VALUE, at, KEY),
                           INTERFACE_CONST_METHOD(std::size_t, size)))
}  // namespace

template <typename KEY, typename VALUE>
void test_map_t_i_template(
    map_t_i<data::has_type_info::const_observer, KEY, VALUE> map_i) {
  REQUIRE(map_i.size() == 2);
  REQUIRE(map_i.at("one") == 1);
  REQUIRE(map_i.at("two") == 2);
}

TEST_CASE("erased template test") {
  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_map_t_i_lambda =
      [](map_t_i<data::has_type_info::const_observer, std::string, int> map_i) {
        REQUIRE(map_i.size() == 2);
        REQUIRE(map_i.at("one") == 1);
        REQUIRE(map_i.at("two") == 2);
      };
  test_map_t_i_lambda(map_string_to_int);

  test_map_t_i_template<std::string, int>(map_string_to_int);
}