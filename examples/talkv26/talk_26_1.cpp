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

namespace External_Polymorphism_Dynamic_Memory_1 {
using namespace anyxx;

template <typename Self>
struct Shape : cloneable<Self> {
  double area() const;
  void inflate(double factor);
};

struct Circle {
  double r_;
  double area() const { return M_PI * r_ * r_; }
  void inflate(double factor) { r_ *= factor; }
};

any_<unique, Shape> inflate_shape(any_<cref, Shape> const& s, double factor) {
  auto result = *clone_to<any_<unique, Shape>>(s);
  result.inflate(factor);
  return result;
}

TEST_CASE("talk_23_1 external dynamic memory 1") {
  any_<shared, Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified.area() == M_PI * 10.0 * 10.0);
}

}  // namespace External_Polymorphism_Dynamic_Memory_1

namespace Internal_Polymorphism_Dynamic_Memory_Polymorphic {

struct Shape {
  virtual ~Shape() = default;
  virtual double area() const = 0;
  virtual void inflate(double) = 0;
};
struct Circle : Shape {
  double r_;
  Circle(double r) : r_(r) {}
  double area() const override { return M_PI * r_ * r_; }
  void inflate(double factor) override { r_ *= factor; }
};

std::polymorphic<Shape> inflate_shape(std::polymorphic<Shape> const& s,
                                      double factor) {
  auto result = s;
  result->inflate(factor);
  return result;
}

TEST_CASE("talk_23_1 internal dynamic memory 2") {
  std::polymorphic<Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified->area() == M_PI * 10.0 * 10.0);
}

}  // namespace Internal_Polymorphism_Dynamic_Memory_Polymorphic

}  // namespace
