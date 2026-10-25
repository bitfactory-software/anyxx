#include <algorithm>
#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/factory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <format>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

using namespace anyxx;

namespace {

struct data {};

TRAIT(source, dynamic_castable, )

TRAIT(greeting, dynamic_castable, 
  (FN(std::string, greet, (), const))
)

TRAIT_MAP(data, greeting) {
  std::string greet(const data&) { return "Hello"; };
};

castable_to<data, greeting> __;

}  // namespace

TEST_CASE("talk_23_8_crosscast") {
  auto d = data{};
  auto s = any<source>{d};
  if (auto g = borrow_as<any<greeting>>(s)) {
    CHECK(g->greet() == "Hello");
  } else {
    FAIL(std::format("crosscast from {} to {} failed", g.error().from.name(),
                     g.error().to.name()));
  }
}
