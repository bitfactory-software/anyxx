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

namespace Internal_Polymorphism_Dynamic {

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

TEST_CASE("talk_26_1 internal dynamic memory polymorphic") {
  std::polymorphic<Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified->area() == M_PI * 10.0 * 10.0);
}

}

namespace External_Polymorphism_Dynamic {

namespace _1 {

using namespace anyxx;

template <typename S>
struct Shape :  copyable<S>{
    double area() const;
    void inflate(double);
};

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

TEST_CASE("talk_26_1 external dynamic memory val 1") {
  any_<val<>, Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified.area() == M_PI * 10.0 * 10.0);
}

}

namespace _2 {

using namespace anyxx;

template <typename S>
struct Shape :  copyable<S>{
    double area() const;
    void inflate(double);
};

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

TEST_CASE("talk_26_1 external dynamic memory val 2") {
  any<Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified.area() == M_PI * 10.0 * 10.0);
}

}


}  // namespace
