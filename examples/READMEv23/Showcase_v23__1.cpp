#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace {
struct circle {
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

TRAIT(drawable, anyxx::dynamic_copyable, (FN(std::string, draw, (), const)))

void draw(std::stringstream& os,
          std::vector<anyxx::any<drawable>> const& drawables) {
  for (auto const& drawable : drawables) os << drawable.draw() << "\n";
}

TEST_CASE("Showcase1") {
  std::stringstream ss;
  draw(ss, {circle{}, square{}});
  CHECK(ss.str() == "Hello\nWorld\n");
}
}  // namespace
