#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/vany.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <ranges>
#include <string>
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

void draw_visted(std::stringstream& os,
    std::vector<known_shapes> const& drawables) {
    for(auto const& drawable : drawables) {
        std::visit([&](auto const& d){ os << d.draw() << "\n"; }, drawable);
    }
}

}

TEST_CASE("talk_2311_variant_visit") {
    std::stringstream ss;
    draw_visted(ss, {circle{}, square{}});
    CHECK(ss.str() == "circle\nsquare\n");
}

namespace {
TRAIT(drawable, anyxx::copyable, (FN(std::string, draw, (), const)))

void draw_trait_as(std::stringstream& os,
    std::vector<trait_as<known_shapes, drawable>> const& drawables) {
    for(auto const& drawable : drawables) {
        os << drawable.draw() << "\n";
    }
}

}  // namespace

TEST_CASE("talk_2311_trait_as_variant") {
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
    for(auto const& f : figures) os << f.draw() << "\n";
}

MODEL_MAP(std::string, drawable) {
  std::string draw(std::string const& s) { return s; };
};

}  // namespace

TEST_CASE("talk_2311_vany_dispatch") {
  std::stringstream ss;
  draw_vany(ss, {circle{}, square{}, any<drawable>{"In the beginning..."s}});
  CHECK(ss.str() == "circle\nsquare\nIn the beginning...\n");
}

