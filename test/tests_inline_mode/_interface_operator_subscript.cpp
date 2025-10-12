#include <catch.hpp>
#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/observer.hpp>
#include <virtual_void/unique.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void;
using namespace virtual_void;

namespace {
using map_t = std::map<int, std::string>;
using vector_t = std::vector<std::string>;
}  // namespace

namespace {
VV_INTERFACE(map_i_to_string_mutable, (VV_OP_EXACT(std::string&, 1, [], int)))

VV_INTERFACE(map_i_to_string_const_and_mutable,
             (VV_OP_EXACT(std::string&, 1, [], int),
              VV_CONST_OP_EXACT(std::string const&, 2, [], int)))

VV_INTERFACE_(map_i_to_string_const_derived_mutable, map_i_to_string_mutable,
              (VV_CONST_OP_EXACT(std::string const&, 2, [], int)))

}  // namespace

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
  map_i_to_string_const_and_mutable<const_observer> v{vector};
  REQUIRE(v[0] == "hallo");
}
TEST_CASE("const subscript_operator with mutable_observer") {
  vector_t vector{"hallo"};
  map_i_to_string_const_and_mutable<mutable_observer> v{vector};
  REQUIRE(v[0] == "hallo");
}

TEST_CASE("mutable subscript_operator (vector) with mutable_observer") {
  vector_t vector{"hallo", ""};
  map_i_to_string_mutable<mutable_observer> v{vector};
  REQUIRE(v[0] == "hallo");
  v[1] = "world";
  REQUIRE(v[1] == "world");
  REQUIRE(vector[1] == "world");

  map_i_to_string_const_and_mutable<const_observer> v_const{vector};
  REQUIRE(v_const[0] == "hallo");
  REQUIRE(v_const[1] == "world");

  map_i_to_string_const_derived_mutable<const_observer> v_const2{vector};
  REQUIRE(v_const2[0] == "hallo");
  REQUIRE(v_const2[1] == "world");

  map_i_to_string_const_derived_mutable<mutable_observer> v_mutable{vector};
  REQUIRE(v_mutable[0] == "hallo");
  REQUIRE(v_mutable[1] == "world");

  v_mutable[0] = "1";
  v_mutable[1] = "2";

  REQUIRE(v_mutable[0] == "1");
  REQUIRE(v_mutable[1] == "2");
}
