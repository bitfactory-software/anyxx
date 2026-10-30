#define _USE_MATH_DEFINES
#include <algorithm>
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

struct Recatangle {
  double length = 0.0, hight = 0;
};

namespace intro {

double area(Circle const& c) { return M_PI * c.radius * c.radius; };
double area(Recatangle const& r) { return r.length * r.hight; };

template <typename... Shapes>
void print_area_static(Shapes const&... shapes) {
  std::println("Area: {}", (... + area(shapes)));
}

TEST_CASE("talk_23_1 static external polymorphism") {
  print_area_static(Circle{5.0}, Recatangle{3.0, 4.0});
};

class Shape {
  using area_f = auto (*)(void const*) -> double;
  area_f area_;
  void const* data_;

 public:
  template <typename S>
    requires(!std::same_as<std::decay_t<S>, Shape>)
  Shape(S&& s)
      : area_(+[](const void* self) {
          return intro::area(*static_cast<std::decay_t<S> const*>(self));
        }),
        data_(&s) {}
  double area() const { return area_(data_); }
};
void print_area_dynamic(std::vector<Shape> const& shapes) {
  std::println("Area: {}",
               std::ranges::fold_left(shapes, 0.0, [](auto l, Shape const& s) {
                 return l + s.area();
               }));
};

TEST_CASE("talk_23_1 dynamic external polymorphism1") {
  Circle c{5.0};
  Recatangle r{3.0, 4.0};
  print_area_dynamic(std::vector<Shape>{c, r});
};

namespace intro_v_table {

class Shape {
  struct v_table_t {
    auto (*area_)(void const*) -> double;
  };
  template <typename S>
  static inline v_table_t v_table{+[](const void* self) {
    return intro::area(*static_cast<std::decay_t<S> const*>(self));
  }};

  v_table_t const* v_table_;
  void const* data_;

 public:
  template <typename S>
    requires(!std::same_as<std::decay_t<S>, Shape>)
  Shape(S&& s) : v_table_(&v_table<std::decay_t<S>>), data_(&s) {}
  double area() const { return v_table_->area_(data_); }
};
void print_area_dynamic(std::vector<Shape> const& shapes) {
  std::println("Area: {}",
               std::ranges::fold_left(shapes, 0.0, [](auto l, Shape const& s) {
                 return l + s.area();
               }));
};

TEST_CASE("talk_23_1 dynamic external polymorphism2") {
  print_area_dynamic(std::vector<Shape>{Circle{5.0}, Recatangle{3.0, 4.0}});
};

}  // namespace intro_v_table

}  // namespace intro

using namespace anyxx;
TRAIT(Shape, deleteable, (FN(double, area, (), const)))

TRAIT_MAP(Circle, Shape) {
  double area(const Circle& self) { return M_PI * self.radius * self.radius; };
};

TEST_CASE("talk_23_1") {
  std::initializer_list<any_<unique, Shape>> shapes = {Circle{5.0}};
  for (const auto& shape : shapes) {
    std::println("Area: {}", shape.area());
    ;
  }
}

TEST_CASE("talk_23_1_ simplified") {
  std::initializer_list<any<Shape>> shapes = {Circle{5.0}};
  for (const auto& shape : shapes) {
    std::println("Area: {}", shape.area());
  }
}

}  // namespace
