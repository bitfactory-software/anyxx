#define ANYXX_26
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace {
// [slide]
struct circle {
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

template <typename Self>
struct drawable : anyxx::dynamic_copyable<Self> {
    std::string draw() const;
};  

using known_shapes = std::variant<circle, square>;

void draw(
    std::stringstream& os,
    std::vector<anyxx::trait_as<known_shapes, drawable>> const& drawables) {
  for (auto const& drawable : drawables) os << drawable.draw() << "\n";
}

TEST_CASE("Showcase6") {
  std::stringstream ss;
  draw(ss, {circle{}, square{}});
  CHECK(ss.str() == "Hello\nWorld\n");
}
// [slide]
}  // namespace
