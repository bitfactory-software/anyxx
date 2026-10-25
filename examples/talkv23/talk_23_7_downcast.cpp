#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/anys/function.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>
#include <vector>

using namespace anyxx;

namespace {

struct data {};

TRAIT(greeting, dynamic_castable,
    (FN(std::string, greet, (), const))
)

TRAIT_MAP(data, greeting) {
  std::string greet(const data&) { return "Hello"; };
};

}  // namespace

TEST_CASE("talk_23_7_downcast") {
  auto d = data{};
  auto g1 = any<greeting>{d};
  auto a = any<dynamic_castable>{g1};
  if (auto g = downcast_to<any<greeting>>(a)) {
    CHECK(g->greet() == "Hello");
  } else {
    CHECK(!"downcast failed");
  }
}
