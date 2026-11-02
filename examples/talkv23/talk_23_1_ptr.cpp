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
  virtual Shape* clone() const = 0;
  virtual double area() const = 0;
  virtual void inflate(double) = 0;
};
struct Circle : Shape {
  double r_;
  Circle(double r) : r_(r) {}
  Shape* clone() const override { return new Circle(*this); }
  double area() const override { return M_PI * r_ * r_; }
  void inflate(double factor) override { r_ *= factor; }
};

std::unique_ptr<Shape> clone_shape(Shape const* s) {
  return std::unique_ptr<Shape>(s->clone());
}
std::unique_ptr<Shape> inflate_shape(Shape const& s, double factor) {
  auto result = clone_shape(&s);
  result->inflate(factor);
  return result;
}

TEST_CASE("talk_23_1 internal dynamic memory ptr") {
  std::shared_ptr<Shape const> origin =
      std::make_shared<Circle const>(Circle{5.0});
  auto modified = inflate_shape(*origin, 2.0);
  CHECK(modified->area() == M_PI * 10.0 * 10.0);
}

}

namespace External_Polymorphism_Dynamic {
using namespace anyxx;

namespace External_Polymorphism_Dynamic_Memory_1 {

TRAIT(Shape, cloneable, 
    (FN(double, area, (), const), 
    FN(void, inflate, (double), )))

struct Circle {
  double r_;
  double area() const { return M_PI * r_ * r_; }
  void inflate(double factor) { r_ *= factor; }
};

any_<unique, Shape> inflate_shape(any_<cref, Shape> const& s, double factor) {
  auto result = clone_to<any_<unique, Shape>>(s);
  result.inflate(factor);
  return result;
}

TEST_CASE("talk_23_1 external dynamic memory ptr") {
  any_<shared, Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified.area() == M_PI * 10.0 * 10.0);
}

}  // namespace External_Polymorphism_Dynamic_Memory_1

}

}  // namespace
