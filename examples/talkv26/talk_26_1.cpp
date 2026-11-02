#define _USE_MATH_DEFINES
#define ANYXX_26
#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <print>
#include <string>
#include <vector>

namespace {

namespace intro {

struct Circle {
  double radius = 0.0;
};

using namespace anyxx;

template <typename Self>
struct Shape : deleteable<Self> {
  double area() const;
};
template <>
struct Shape<Circle> {
  static double area(const Circle& self) {
    return M_PI * self.radius * self.radius;
  }
};

TEST_CASE("talk_2610_ShapesAny++26") {
  std::initializer_list<any<Shape>> shapes = {Circle{5.0}};
  for (const auto& shape : shapes) {
    std::println("Area: {}", shape.area());
  }
}

}  // namespace intro

namespace External_Polymorphism_Dynamic {

using namespace anyxx;

template <typename Self>
struct Shape {
  double area() const;
};
using any_shape = any_<shared, Shape>;
using any_shapes = std::vector<any_shape>;
void print_area_dyn(any_shapes const& shapes) {
  std::println("Area: {}", std::ranges::fold_left(
                               shapes, 0.0, [](auto sum, any_shape const& s) {
                                 return sum + s.area();
                               }));
}
template <typename... Args>
void print_area_static(trait_as<Args, Shape> const&... shapes) {
  std::println("Area: {}", (shapes.area() + ...));
}

struct Circle {
  double r = 0.0;
};
template <>
struct Shape<Circle> {
  static double area(const Circle& c) { return M_PI * c.r * c.r; }
};
struct Rect {
  double w, h;
};
template <>
struct Shape<Rect> {
  static double area(const Rect& r) { return r.w * r.h; }
};

TEST_CASE("talk_26_1 external static & dynamic") {
  print_area_dyn({Circle{5.0}, Rect{3.0, 4.0}});
  print_area_static(as<Shape>(Circle{5.0}), as<Shape>(Rect{3.0, 4.0}));
}

}  // namespace External_Polymorphism_Dynamic

}  // namespace
