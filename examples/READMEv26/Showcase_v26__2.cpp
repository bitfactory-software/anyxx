#define ANYXX_26
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace {

struct circle {};
struct square {
  [[nodiscard]] std::string edgy_salute() const { return "edgy World"; }
};

template <typename Self>
struct drawable : anyxx::dynamic_copyable<Self> {
    std::string draw() const;
};

template <>
struct drawable<circle> {
  static std::string draw(circle const&) { return "Silent greetings"; };
};

template <>
struct drawable<square> {
  static std::string draw(square const& self) { return self.edgy_salute(); };
};

void draw(std::stringstream& os,
          std::vector<anyxx::any<drawable>> const& drawables) {
  for (auto const& drawable : drawables) os << drawable.draw() << "\n";
}

TEST_CASE("Showcase2") {
  std::stringstream ss;
  draw(ss, {circle{}, square{}});
  CHECK(ss.str() == "Silent greetings\nedgy World\n");
}
};  // namespace showcase2
