#define ANYXX_26
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/vany.hpp>
#include <bit_factory/anyxx/v26/meta/print_members.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

using namespace anyxx;
using namespace std::string_literals;

namespace {

struct circle {
  [[nodiscard]] std::string draw() const { return "circle"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "square"; }
};

using known_shapes = std::variant<circle, square>;

template <typename Self>
struct drawable : copyable<Self> {
  std::string draw() const;
};

void draw_trait_as(std::stringstream& os,
                   std::vector<trait_as<known_shapes, drawable>> drawables) {
  for (auto const& drawable : drawables) {
    os << drawable.draw() << "\n";
  }
}

}  // namespace

TEST_CASE("talk_2611_variant_dispatch") {
  std::stringstream ss;
  draw_trait_as(ss, {circle{}, square{}});
  CHECK(ss.str() == "circle\nsquare\n");
}

namespace {
using known_and_unknown_shapes = vany<drawable, circle, square>;
static_assert(std::same_as<
              known_and_unknown_shapes,
              trait_as<std::variant<any<drawable>, circle, square>, drawable>>);

void draw_vany(std::stringstream& os,
               std::vector<known_and_unknown_shapes> const& figures) {
    for (auto const& f : figures) { 
        os << f.draw() << "\n"; 
    }
}

template <>
struct drawable<std::string> {
  static std::string draw(std::string const& s) { return s; };
};

}  // namespace

TEST_CASE("talk_2611_vany_dispatch") {
  std::stringstream ss;
  draw_vany(ss, {circle{}, square{}, any<drawable>{"The big unknown..."s}});
  CHECK(ss.str() == "circle\nsquare\nThe big unknown...\n");
}
