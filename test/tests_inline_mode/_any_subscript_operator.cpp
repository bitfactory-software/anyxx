#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <map>
#include <string>
#include <vector>

using namespace anyxx;
using namespace anyxx;
using namespace anyxx;

namespace {
namespace subscript_operator_test {

using map_t = std::map<std::size_t, std::string>;
using vector_t = std::vector<std::string>;

TRAIT(map_i_to_string_const_and_mutable, anyxx::dynamic_copyable,
      (ANY_OP_EXACT(std::string&, [], (std::size_t), ),
       ANY_OP_EXACT_MAP_NAMED(std::string const&, [], subscript, (std::size_t),
                              const)))

TRAIT(map_i_to_string_mutable, anyxx::dynamic_copyable,
      (ANY_OP_EXACT(std::string&, [], (std::size_t), )))
TRAIT(map_i_to_string_const_derived_mutable, map_i_to_string_mutable,
     (ANY_OP_EXACT_OVERLOAD_DEF(
         public, std::string const&, [], subscript, (std::size_t), const,
         [&x](std::size_t i) -> std::string const& { return x.at(i); })))

}  // namespace subscript_operator_test
}  // namespace

using namespace subscript_operator_test;

MODEL_MAP(subscript_operator_test::map_t,
              subscript_operator_test::map_i_to_string_const_and_mutable) {
  static std::string const& subscript(map_t const& self,
                                      std::size_t const key) {  // NOLINT
    return self.at(key);
  };
};

MODEL_MAP(subscript_operator_test::vector_t,
              subscript_operator_test::map_i_to_string_const_and_mutable) {
  static std::string const& subscript(vector_t const& self,
                                      std::size_t const key) {  // NOLINT
    return self.at(key);
  };
};

TEST_CASE("mutable subscript_operator with mutref") {
  map_t map{{0, "hallo"}};
  any_<mutref, map_i_to_string_mutable> m{map};
  REQUIRE(m[0u] == "hallo");
  m[1u] = "world";
  REQUIRE(m[1u] == "world");
  REQUIRE(map[1u] == "world");
}

TEST_CASE("const subscript_operator with cref") {
  vector_t vector{"hallo"};
  any_<cref, map_i_to_string_const_and_mutable> v{vector};
  REQUIRE(v[0u] == "hallo");
}
TEST_CASE("const subscript_operator with mutref") {
  vector_t vector{"hallo"};
  any_<mutref, map_i_to_string_const_and_mutable> v{vector};
  REQUIRE(v[0u] == "hallo");
}

TEST_CASE("mutable subscript_operator (vector) with mutref") {
  vector_t vector{"hallo", ""};
  any_<mutref, map_i_to_string_mutable> v{vector};
  REQUIRE(v[0u] == "hallo");
  v[1u] = "world";
  REQUIRE(v[1u] == "world");
  REQUIRE(vector[1u] == "world");

  any_<cref, map_i_to_string_const_and_mutable> v_const{vector};
  REQUIRE(v_const[0u] == "hallo");
  REQUIRE(v_const[1u] == "world");

  any_<cref, map_i_to_string_const_derived_mutable> v_const2{vector};
  REQUIRE(v_const2[0u] == "hallo");
  REQUIRE(v_const2[1u] == "world");

  any_<mutref, map_i_to_string_const_derived_mutable> v_mutable{vector};
  REQUIRE(v_mutable[0u] == "hallo");
  REQUIRE(v_mutable[1u] == "world");

  v_mutable[0u] = "1";
  v_mutable[1u] = "2";

  REQUIRE(v_mutable[0u] == "1");
  REQUIRE(v_mutable[1u] == "2");
}
