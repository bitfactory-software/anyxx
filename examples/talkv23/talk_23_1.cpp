#define _USE_MATH_DEFINES
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <print>
#include <string>
#include <vector>

namespace {

struct Circle {
  double radius;
};

using namespace anyxx;
TRAIT(Shape, deleteable, (FN(double, area, (), const)))

MODEL_MAP((Circle), Shape) {
  double area(const Circle& self) { return M_PI * self.radius * self.radius; };
};

TEST_CASE("ShapesAny++23") {
  std::initializer_list<any_<unique, Shape>> shapes = {Circle{5.0}};
  for (const auto& shape : shapes) {
    std::println("Area: {}", shape.area());
    ;
  }
}

TEST_CASE("ShapesAny++23 simplified") {
  std::initializer_list<any<Shape>> shapes = {Circle{5.0}};
  for (const auto& shape : shapes) {
    std::println("Area: {}", shape.area());
  }
}

}  // namespace
