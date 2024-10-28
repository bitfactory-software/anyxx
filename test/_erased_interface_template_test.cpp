#include <cmath>
#include <iostream>
#include <map>
#include <string>

#include "../include/virtual_void/erased/data/has_no_meta/observer.h"
#include "../include/virtual_void/erased/interface/declare_macro.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace ::virtual_void::erased;

const double M_PI = 3.14;

struct position {
  float x, y;
};

struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

//namespace {
//ERASED_INTERFACE(node_si_i, (INTERFACE_CONST_METHOD(const VALUE&, at, KEY),
//                             INTERFACE_CONST_METHOD(std::size_t, size)))
//ERASED_INTERFACE_TEMPLATE((KEY, VALUE), map_t_i,
//                          (INTERFACE_CONST_METHOD(const VALUE&, at, KEY),
//                           INTERFACE_CONST_METHOD(std::size_t, size)))
//}  // namespace
//
//TEST_CASE("erased template test") {
//  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};
//
//  auto test_map_t_i =
//      [](map_t_i<data::has_no_meta::const_observer, std::string, int> map_i) {
//        REQUIRE(map_i.size() == 1);
//        REQUIRE(map_i.at("one") == 1);
//        REQUIRE(map_i.at("two") == 2);
//      }(map_string_to_int);
}