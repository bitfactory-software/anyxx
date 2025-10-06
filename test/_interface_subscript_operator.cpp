#include <catch.hpp>
#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data;

namespace {
using map_t = std::map<int, std::string>;
using vector_t = std::vector<std::string>;
}  // namespace

namespace {
VV_INTERFACE(map_i_to_string_const, (VV_CONST_OP(std::string const&, 1, [], int)))
VV_INTERFACE(map_i_to_string_mutable,
             (VV_OP(std::string&, 1, [], int))) /*,
               VV_CONST_OP(std::string const&, [], int)*/

}  // namespace

VV_RUNTIME_STATIC(map_t)
VV_V_TABLE_INSTANCE(, map_t, map_i_to_string_mutable)
VV_RUNTIME_STATIC(vector_t)
VV_V_TABLE_INSTANCE(, vector_t, map_i_to_string_const)
VV_V_TABLE_INSTANCE(, vector_t, map_i_to_string_mutable)

TEST_CASE("mutable subscript_operator with mutable_observer") {
  map_t map{{0, "hallo"}};
  map_i_to_string_mutable<mutable_observer> m{map};
  REQUIRE(m[0] == "hallo");
  m[1] = "world";
  REQUIRE(m[1] == "world");
  REQUIRE(map[1] == "world");
}

TEST_CASE("const subscript_operator with const_observer") {
  vector_t vector{"hallo"};
  map_i_to_string_const<const_observer> v{vector};
  REQUIRE(v[0] == "hallo");
}
TEST_CASE("const subscript_operator with mutable_observer") {
  vector_t vector{"hallo"};
  map_i_to_string_const<mutable_observer> v{vector};
  REQUIRE(v[0] == "hallo");
}

TEST_CASE("mutable subscript_operator (vector) with mutable_observer") {
  vector_t vector{"hallo", ""};
  map_i_to_string_mutable<mutable_observer> v{vector};
  REQUIRE(v[0] == "hallo");
  v[1] = "world";
  REQUIRE(v[1] == "world");
  REQUIRE(vector[1] == "world");

  // map_i_to_string_mutable<const_observer> v_const{vector};
  // REQUIRE(v_const[0] == "hallo");
  // v[1] = "world";
  // REQUIRE(v[1] == "world");
  // REQUIRE(vector[1] == "world");
}
