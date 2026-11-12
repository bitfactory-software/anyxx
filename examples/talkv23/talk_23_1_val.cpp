#define _USE_MATH_DEFINES
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <print>
#include <string>
#include <vector>

namespace {

namespace External_Polymorphism_Dynamic_Memory {

namespace _1 {

using namespace anyxx;

TRAIT(Shape, copyable, 
    (FN(double, area, (), const), 
    FN(void, inflate, (double), )))

struct Circle {
  double r_;
  double area() const { return M_PI * r_ * r_; }
  void inflate(double factor) { r_ *= factor; }
};

any_<val<>, Shape> inflate_shape(any_<val<>, Shape> const& s, double factor) {
  auto result = s;
  result.inflate(factor);
  return result;
}

TEST_CASE("talk_23_1 external dynamic memory val 1") {
  any_<val<>, Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified.area() == M_PI * 10.0 * 10.0);
}

}

namespace _2 {

using namespace anyxx;

TRAIT(Shape, copyable, 
    (FN(double, area, (), const), 
    FN(void, inflate, (double), )))

struct Circle {
  double r_;
  double area() const { return M_PI * r_ * r_; }
  void inflate(double factor) { r_ *= factor; }
};

any<Shape> inflate_shape(any<Shape> const& s, double factor) {
  auto result = s;
  result.inflate(factor);
  return result;
}

TEST_CASE("talk_23_1 external dynamic memory val 2") {
  any<Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified.area() == M_PI * 10.0 * 10.0);
}

}

}

}  // namespace
