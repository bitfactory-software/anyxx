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

template <is_trait Trait, typename Self>
struct Shape {
  double area() const;
};

void print_area(const any<Shape>& shape) {
  std::println("Area: {}", shape.area());
}

template <>
struct Shape<model_map, Circle> {
  static double area(const Circle& self) {
    return M_PI * self.radius * self.radius;
  }
};

TEST_CASE("ShapesAny++26 cref") {
  Circle circle{5.0};
  print_area(circle);
}

}  // namespace
