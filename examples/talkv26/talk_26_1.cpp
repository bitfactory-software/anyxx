#define _USE_MATH_DEFINES
#define ANYXX_26
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <print>
#include <string>
#include <vector>

namespace {

struct Circle {
  double radius = 0.0;
};

using namespace anyxx;

template <is_trait Trait, typename Self>
struct Shape : deleteable<Trait, Self> {
  double area() const;
};
template <>
struct Shape<model_map, Circle> {
  static double area(const Circle& self) { return M_PI * self.radius * self.radius; }
};

TEST_CASE("talk_2610_ShapesAny++26") {
  std::initializer_list<any<Shape>> shapes = {Circle{5.0}};
  for (const auto& shape : shapes) {
    std::println("Area: {}", shape.area());
  }
}

}  // namespace
