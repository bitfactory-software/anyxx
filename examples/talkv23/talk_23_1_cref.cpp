#define _USE_MATH_DEFINES
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <print>
#include <string>
#include <vector>

namespace {

namespace Internal_Polymorphism_Dynamic {

struct Shape {
  virtual ~Shape() = default;
  virtual double area() const = 0;
};
struct Circle : Shape {
  double r_;
  Circle(double r) : r_(r) {}
  double area() const override { return M_PI * r_ * r_; }
};

double f(Shape const& s) { return s.area(); }

TEST_CASE("talk_23_1 internal dynamic memory ref") {
  Circle c{5.0};
  CHECK(f(c) == M_PI * 5.0 * 5.0);
}

}

namespace External_Polymorphism_Dynamic {
using namespace anyxx;

TRAIT(Shape, observeable, 
    (FN(double, area, (), const)))

struct Circle {
  double r_;
  double area() const { return M_PI * r_ * r_; }
};

namespace _1 {
double f(any_<cref, Shape> const& s) { return s.area(); }

TEST_CASE("talk_23_1 external dynamic memory cref 1") {
  Circle c{5.0};
  CHECK(f(c) == M_PI * 5.0 * 5.0);
}
}

namespace _2 {

double f(any<Shape> const& s) { return s.area(); }

TEST_CASE("talk_23_1 external dynamic memory cref 2") {
  Circle c{5.0};
  CHECK(f(c) == M_PI * 5.0 * 5.0);
}

}

}

}  // namespace
