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

ANY(map_i_to_string_const_and_mutable,
    (ANY_OP_EXACT(std::string&, [], (std::size_t), ),
     ANY_OP_EXACT_MAP_NAMED(std::string const&, [], subscript, (std::size_t),
                            const)),
    , )

ANY(map_i_to_string_mutable, (ANY_OP_EXACT(std::string&, [], (std::size_t), )),
    , )
ANY_(map_i_to_string_const_derived_mutable, map_i_to_string_mutable,
     (ANY_OP_EXACT_OVERLOAD_DEFAULTED(
         std::string const&, [], subscript, (std::size_t), const,
         [&x](std::size_t i) -> std::string const& { return x.at(i); })),
     , )

}  // namespace subscript_operator_test
}  // namespace

using namespace subscript_operator_test;

ANY_MODEL_MAP((subscript_operator_test::map_t),
              subscript_operator_test::map_i_to_string_const_and_mutable) {
  static std::string const& subscript(map_t const& self,
                                      std::size_t const key) {  // NOLINT
    return self.at(key);
  };
};

ANY_MODEL_MAP((subscript_operator_test::vector_t),
              subscript_operator_test::map_i_to_string_const_and_mutable) {
  static std::string const& subscript(vector_t const& self,
                                      std::size_t const key) {  // NOLINT
    return self.at(key);
  };
};

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
