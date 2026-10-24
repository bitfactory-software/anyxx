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
TRAIT(Shape, copyable, (FN(double, area, (), const)))

void print_area(const any<Shape>& shape) {
  std::println("Area: {}", shape.area());
}

TRAIT_MAP(Circle, Shape) {
  double area(const Circle& self) { return M_PI * self.radius * self.radius; };
};

TEST_CASE("talk_2310_ShapesAny++23 val") {
  print_area(Circle{ 5.0 });
}

}  // namespace
