#define ANYXX_26
#include <bit_factory/anyxx/ext/vany.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace showcase7 {
using namespace anyxx;
using namespace std::string_literals;

struct circle {
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

template <typename Self>
struct figure : anyxx::dynamic_copyable<Self> {
    std::string draw() const;
};

using known_and_unknown_shapes = vany<figure, circle, square>;
static_assert(std::same_as<
              known_and_unknown_shapes,
              trait_as<std::variant<any<figure>, circle, square>, figure>>);
void draw_vany(std::stringstream& os,
    std::vector<known_and_unknown_shapes> const& figures) {
    for(auto const& f : figures) os << f.draw() << "\n";
}

template <> 
struct figure<std::string> {
  static std::string draw(std::string const& s) { return s; };
};

TEST_CASE("Showcase7") {
  std::stringstream ss;
  draw_vany(ss, {circle{}, square{}, any<figure>{"The big unknown..."s}});
  CHECK(ss.str() == "Hello\nWorld\nThe big unknown...\n");
}
}  // namespace showcase7
