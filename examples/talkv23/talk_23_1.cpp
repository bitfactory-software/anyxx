#define _USE_MATH_DEFINES
#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <iostream>
#include <map>
#include <print>
#include <sstream>
#include <string>
#include <vector>

namespace {

namespace Internal_Polymorphism_Dynamic {

struct Shape {
  virtual ~Shape() = default;
  virtual double area() const = 0;
};
using shape_ptr = std::shared_ptr<Shape const>;
using shapes_vec = std::vector<shape_ptr>;
void print_area(shapes_vec const& shapes) {
  std::println("Area: {}", std::ranges::fold_left(
                               shapes | std::views::transform(&Shape::area),
                               0.0, std::plus{}));
}
struct Circle : Shape {
  double r_;
  Circle(double r) : r_(r) {}
  double area() const override { return M_PI * r_ * r_; }
};
struct Rect : Shape {
  double w_, h_;
  Rect(double w, double h) : w_(w), h_(h) {}
  double area() const override { return w_ * h_; }
};

TEST_CASE("talk_23_1 internal dynamic") {
  print_area({std::make_shared<Circle>(5.0), std::make_shared<Rect>(3.0, 4.0)});
}

using shape_factory = shape_ptr (*)(std::istream&);
std::map<std::string, shape_factory> shape_factories;
shapes_vec load_shapes(std::istream& shapes_archive);

TEST_CASE("talk_23_1 internal dynamic ioc") {
  std::stringstream is("Circle 5.0\nRect 3.0 4.0\n");
  print_area(load_shapes(is));
}

auto __ = shape_factories["Circle"] = [](std::istream& is) -> shape_ptr {
  double r;
  is >> r;
  return std::make_shared<Circle>(r);
};
auto __ = shape_factories["Rect"] = [](std::istream& is) -> shape_ptr {
  double w, h;
  is >> w >> h;
  return std::make_shared<Rect>(w, h);
};

shape_ptr load_shape(std::istream& shape_archive) {
  std::string shape_type;
  shape_archive >> shape_type;
  auto it = shape_factories.find(shape_type);
  if (it != shape_factories.end()) {
    return it->second(shape_archive);
  }
  return nullptr;
}
shapes_vec load_shapes(std::istream& shapes_archive) {
  shapes_vec result;
  while (auto s = load_shape(shapes_archive)) {
    result.push_back(s);
  }
  return result;
}

}  // namespace Internal_Polymorphism_Dynamic

namespace External_Polymorphism_Static {

template <typename S>
concept Shape = requires(S s) {
  { area(s) } -> std::convertible_to<double>;
};
template <Shape... Shapes>
void print_area(Shapes const&... shapes) {
  std::println("Area: {}", (area(shapes) + ...));
}

struct Circle {
  double r;
};
double area(Circle const& c) { return M_PI * c.r * c.r; };
static_assert(Shape<Circle>);
struct Rect {
  double w, h;
};
double area(Rect const& r) { return r.w * r.h; };
static_assert(Shape<Rect>);

TEST_CASE("talk_23_1 external static concept") {
  print_area(Circle{5.0}, Rect{3.0, 4.0});
}

}  // namespace External_Polymorphism_Static

namespace External_Polymorphism_Static_Dynamic {

using namespace anyxx;
TRAIT(Shape, observeable, (FN(double, area, (), const)))
void print(std::vector<any_<shared, Shape>> const& shapes) {
  for (auto const& s : shapes) {
    std::println("Area dyn: {}", s.area());
  }
}
template <typename... Args>
void print_(trait_as<Args, Shape> const&... shapes) {
  (std::println("Area : {}", shapes.area()), ...);
}

struct Circle {
  double r = 0.0;
};
TRAIT_MAP(Shape, Circle) {
  double area(const Circle& self) { return M_PI * self.r * self.r; };
};
struct Rect {
  double w, h;
};
TRAIT_MAP(Shape, Rect) {
  double area(const Rect& self) { return self.w * self.h; };
};

TEST_CASE("talk_236_1 external static & dynamic") {
  print({Circle{5.0}, Rect{3.0, 4.0}});
  print_(as<Shape>(Circle{5.0}), as<Shape>(Rect{3.0, 4.0}));
}
}  // namespace External_Polymorphism_Static_Dynamic

}  // namespace
