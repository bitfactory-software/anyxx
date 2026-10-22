#define ANYXX_26
#include <bit_factory/anyxx.hpp>
// #include <bit_factory/anyxx/ext/vany.hpp>
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
  [[nodiscard]] std::string draw() const { return "Hello"; }
};
struct square {
  [[nodiscard]] std::string draw() const { return "World"; }
};

template <is_trait Trait, typename Self>
struct drawable : copyable<Trait, Self> {
  std::string draw() const;
};

// template <typename... Vs>
// struct drawable<model_map, std::variant<Vs...>> {
//   static std::string draw(std::variant<Vs...> const& v) {
//     return std::visit(
//         [](auto const& x) {
//           static_assert(std::is_same_v<decltype(x), circle> ||
//                         std::is_same_v<decltype(x), square>);
//           return trait_as<decltype(x), drawable>(x).draw();
//         },
//         v);
//   }
// };

using known_shapes = std::variant<circle, square>;

// consteval{
//   constexpr auto spec = interface_spec{^^drawable<declaration, declaration>,
//   ^^drawable<declaration, declaration>::draw, 0u}; constexpr auto target =
//   find_candidate_in<circle, spec.member>(); static_assert(target ==
//   ^^std::string);
// }

// void draw(std::stringstream& os,
//           std::vector<trait_as<known_shapes, drawable>> const& drawables) {
//   for (auto const& drawable : drawables) {
//     os << drawable.draw() << "\n";
//   }
// }

}  // namespace

TEST_CASE("talk_2311_variant_dispatch") {
  // consteval {
  //    constexpr auto spec = ^^drawable<model_map, circle>::draw;
  //    static_assert(is_defaulted_function_spec(spec));
  //    static_assert(is_defaulted_function_spec(^^drawable<model_map,
  //    circle>::draw));
  // }
  // static_assert(!is_defaulted_function_spec(^^drawable<model_map,
  // circle>::draw));
  auto s1 = trait_as<circle, drawable>(circle{}).draw();
  CHECK(s1 == "Hello");

  // std::stringstream ss;
  // draw(ss, {known_shapes{circle{}}, known_shapes{square{}}});
  // CHECK(ss.str() == "Hello\nWorld\n");
}

// namespace {
// using known_and_unknown_shapes = make_vany<drawable, circle, square>;
// static_assert(std::same_as<
//               known_and_unknown_shapes,
//               trait_as<std::variant<any<drawable>, circle, square>,
//               drawable>>);
//
// void draw_vany(std::stringstream& os,
//     std::vector<known_and_unknown_shapes> const& figures) {
//     for(auto const& f : figures) os << f.draw() << "\n";
// }
//
// MODEL_MAP(std::string, drawable) {
//   std::string draw(std::string const& s) { return s; };
// };
//
// }  // namespace
//
// TEST_CASE("talk_2311_vany_dispatch") {
//   std::stringstream ss;
//   draw_vany(ss, {circle{}, square{}, any<drawable>{"The big unknown..."s}});
//   CHECK(ss.str() == "Hello\nWorld\nThe big unknown...\n");
// }
//
