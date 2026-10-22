#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <ranges>
#include <string>
#include <vector>

using namespace anyxx;

namespace {

struct circle {
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

TRAIT(drawable, anyxx::copyable, (FN(std::string, draw, (), const)))

using known_shapes = std::variant<circle, square>;

void draw(std::stringstream& os,
          std::vector<trait_as<known_shapes, drawable>> const& drawables) {
  for (auto const& drawable : drawables) {
    os << drawable.draw() << "\n";
  }
}

}

TEST_CASE("talk_2311_variant_dispatch") {
  std::stringstream ss;
  draw(ss, {circle{}, square{}});
  CHECK(ss.str() == "Hello\nWorld\n");
}
