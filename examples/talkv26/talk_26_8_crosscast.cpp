#define ANYXX_26
#include <algorithm>
#include <array>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>


namespace {
// [slide]
using namespace anyxx;

struct data {};

template <typename Self>
struct greeting : dynamic_castable<Self> {
  std::string greet() const;
};

template <>
struct greeting<data> {
  static std::string greet(const data&) { return "Hello"; };
};

castable_to<data, greeting> __;

TEST_CASE("talk_23_8_crosscast") {
  auto d = data{};
  auto s = any<dynamic_castable>{d};
  if (auto g = borrow_as<any<greeting>>(s)) {
    CHECK(g->greet() == "Hello");
  } else {
    FAIL(std::format("crosscast from {} to {} failed", g.error().from.name(),
                     g.error().to.name()));
  }
}
// [slide]

}  // namespace
