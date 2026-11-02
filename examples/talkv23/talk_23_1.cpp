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
                               shapes, 0.0, [](auto sum, shape_ptr const& s) {
                                 return sum + s->area();
                               }));
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

double f(Shape const& s) { return s.area(); }

TEST_CASE("talk_23_1 internal dynamic memory 1") {
  Circle c{5.0};
  CHECK(f(c) == M_PI * 5.0 * 5.0);
}

}  // namespace Internal_Polymorphism_Dynamic

namespace Internal_Polymorphism_Dynamic_Memory {

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

TEST_CASE("talk_23_1 internal dynamic memory 2") {
  std::shared_ptr<Shape const> origin =
      std::make_shared<Circle const>(Circle{5.0});
  auto modified = inflate_shape(*origin, 2.0);
  CHECK(modified->area() == M_PI * 10.0 * 10.0);
}

}  // namespace Internal_Polymorphism_Dynamic_Memory

namespace External_Polymorphism_Static {

template <typename S>
concept Shape = requires(S s) {
  { area(s) } -> std::convertible_to<double>;
};
template <typename... Shapes>
void print_area(Shapes const&... shapes) {
  std::println("Area: {}", (area(shapes) + ...));
}

struct Circle {
  double r;
};
double area(Circle const& c) { return M_PI * c.r * c.r; };

struct Rect {
  double w, h;
};
double area(Rect const& r) { return r.w * r.h; };

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
TRAIT_MAP(Circle, Shape) {
  double area(const Circle& self) { return M_PI * self.r * self.r; };
};
struct Rect {
  double w, h;
};
TRAIT_MAP(Rect, Shape) {
  double area(const Rect& self) { return self.w * self.h; };
};

TEST_CASE("talk_236_1 external static & dynamic") {
  print({Circle{5.0}, Rect{3.0, 4.0}});
  print_(as<Shape>(Circle{5.0}), as<Shape>(Rect{3.0, 4.0}));
}
}  // namespace External_Polymorphism_Static_Dynamic

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

TEST_CASE("talk_23_1 external dynamic memory 1") {
  any_<shared, Shape> origin{std::in_place_type<Circle>, 5.0};
  auto modified = inflate_shape(origin, 2.0);
  CHECK(modified.area() == M_PI * 10.0 * 10.0);
}

}  // namespace External_Polymorphism_Dynamic_Memory_1

}  // namespace
