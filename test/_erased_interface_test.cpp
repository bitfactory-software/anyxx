#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/interface.h"
#include "../include/virtual_void/erased/lifetime/observer.h"
#include "../include/virtual_void/erased/lifetime/value.h"
#include "../include/virtual_void/m_table/lifetime/shared_const.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;

const double M_PI = 3.14;

struct position {
  float x, y;
};

ERASED_INTERFACE(shape_base1, INTERFACE_METHOD(void, draw, position))

ERASED_INTERFACE_(shape_base, shape_base1, INTERFACE_METHOD(int, count_sides) )

ERASED_INTERFACE_(shape_d_i, shape_base, INTERFACE_METHOD(double, area), INTERFACE_METHOD(double, perimeter))

ERASED_INTERFACE(shape_i, INTERFACE_METHOD(void, draw, position),
                 INTERFACE_METHOD(int, count_sides),
                 INTERFACE_METHOD(double, area),
                 INTERFACE_METHOD(double, perimeter))

using shape_vv = shape_i<virtual_void::m_table::shared_const>;

using shape_base_v = shape_base<erased::const_observer>;

using shape =
    virtual_void::erased::call_operator<std::string(std::string),
                                        erased::const_observer, shape_d_i>;
using shapeX = shape_d_i<erased::const_observer>;
using shapeXX = shape_d_i<erased::const_observer>;

using full_shape_observer = shape_i<erased::mutable_observer>;

struct circle {
  double radius;
  void draw(position p) const {
    std::cout << " A Circle Is Recorded At " << p.x << " " << p.y << std::endl;
  }
  int count_sides() const { return 1; }
  double area() const { return radius * radius * M_PI; }
  double circumference() const { return radius * 2.0f * M_PI; }
  double perimeter() const { return circumference(); }
  std::string operator()(const std::string& x) const { return x + "circle"; }
};
struct square {
  int w;
  void draw(position p) const {
    std::cout << " A Square Is Recorded At " << p.x << " " << p.y << std::endl;
  }
  int count_sides() const { return 4; }
  double area() const { return w * w; }
  double perimeter() const { return w * 4; }
  std::string operator()(const std::string& x) const { return x + "square"; }
};
struct rectangle {
  int w, h;
  void draw(position p) const {
    std::cout << " A Rectangle Is Recorded At " << p.x << " " << p.y
              << std::endl;
  }
  int count_sides() const { return 4; }
  double area() const { return w * h; }
  double perimeter() const { return w + w + h + h; }
  std::string operator()(const std::string& x) const { return x + "rectangle"; }
};
struct regular_polygon {
  int sides;
  double side_length;
  void draw(position p) const {
    std::cout << " A Polygon Is Recorded At " << p.x << ", " << p.y
              << std::endl;
  }
  int count_sides() const { return sides; }
  double apothem() const { return (side_length / 2) / std::tan(M_PI / sides); }
  double radius() const { return (side_length / 2) / std::sin(M_PI / sides); }
  double perimeter() const { return sides * side_length; }
  double area() const { return (perimeter() * apothem()) / 2; }
  std::string operator()(const std::string& x) const {
    return x + "regular_polygon";
  }
};

void print_shape_(const auto s) {
  s.draw({4.0, 5.0});
  std::cout << "Shape Number Of Sides: " << s.count_sides() << std::endl;
  std::cout << "Shape Perimeter: " << s.perimeter() << std::endl;
  std::cout << "Shape Area: " << s.area() << std::endl;
}
void print_shape_vv(const shape_vv s) { print_shape_(s); }
void print_shape(const shape s) {
  print_shape_(s);
  std::cout << s("Shape type = ") << std::endl;
}
void print_shape_f(const full_shape_observer s) { print_shape_(s); }

// using shape_double_base_error = shape_d_i< erased::const_observer,
// virtual_void::erased::bases< shape_base, shape_base > >; //should not
// compile! void should_not_compile(shape_double_base_error s) {}//should not
// compile!

TEST_CASE("dynamic interface const_observer") {
  using namespace virtual_void;

  circle c{12.3};
  square s{32};
  rectangle r{12, 9};
  regular_polygon p{4, 32};

  std::cout << "print_shape *******************************" << std::endl;

  print_shape(c);
  print_shape(s);
  print_shape(r);
  print_shape(p);

  static_assert(
      std::is_base_of_v<virtual_void::erased::base<erased::const_observer>,
                        shape>);
  static_assert(std::is_base_of_v<shape_base_v, shape>);
  static_assert(std::derived_from<shape, shape_base_v>);
  shape shape_circle{circle{33.3}};
  shapeX shape_circleX{circle{33.3}};

  //    virtual_void::erased::base< void* > base_v = shape_circle; ->
  //    interface_cast may not compile!
  virtual_void::erased::base<erased::const_observer> base_shape = shape_circle;
  virtual_void::erased::base<erased::const_observer> base_shapeX =
      shape_circleX;

  REQUIRE(base_shape.is_derived_from<shape>());
  REQUIRE(base_shape.is_derived_from<shapeX>());
  REQUIRE(!shape_circleX.is_derived_from<shape>());
  REQUIRE(shape_circleX.is_derived_from<shapeX>());
  static_assert(std::derived_from<shape, shape_base_v>);
  static_assert(std::derived_from<shapeX, shape_base_v>);
  REQUIRE(virtual_void::erased::interface_cast<shapeX>(base_shape));
  REQUIRE(!virtual_void::erased::interface_cast<shape>(shape_circleX));

  shape_base_v shape_circle_base = shape_circle;
  {
    shape shape_is_circle =
        virtual_void::erased::static_interface_cast<shape>(shape_circle_base);
    print_shape(shape_is_circle);
  }
  {
    auto shape_is_circle =
        virtual_void::erased::interface_cast<shape>(shape_circle_base);
    REQUIRE(shape_is_circle);
    print_shape(*shape_is_circle);
  }

  print_shape_f(full_shape_observer{p});
}

TEST_CASE("dynamic interface m_table::shared_const") {
  using namespace virtual_void;

  circle c{12.3};
  square s{32};
  rectangle r{12, 9};
  regular_polygon p{4, 32};
  std::cout << "print_shape_vv ********************************" << std::endl;

  auto sc = m_table::make_shared_const<circle>(circle{c});
  static_assert(
      std::is_same_v<decltype(sc), m_table::typed_shared_const<circle> >);
  static_assert(std::is_base_of_v<m_table::shared_const,
                                  m_table::typed_shared_const<circle> >);
  auto& c1 = *sc;
  REQUIRE_THAT(c1.perimeter(), WithinAbs(77.2, 77.3));
  shape_vv circle_shape_vv = sc;
  auto unerased_circle = as<circle>(circle_shape_vv.get_lifetime_holder());
  REQUIRE_THAT(unerased_circle->perimeter(), WithinAbs(77.2, 77.3));
  auto x = circle_shape_vv;
  REQUIRE_THAT(circle_shape_vv.perimeter(), WithinAbs(77.2, 77.3));
  print_shape_vv(sc);
  print_shape_vv(circle_shape_vv);
  print_shape_vv(c);
  print_shape_vv(s);
  print_shape_vv(r);
  print_shape_vv(p);
}

TEST_CASE("base") {
  using namespace virtual_void;
  using namespace virtual_void::erased;

  using value_base = base<value>;

  struct x_t {
    std::string s_;
  };
  {
    x_t a{"hallo"};
    value erased = erase_to<value>(a);
    REQUIRE(reconcrete_cast<x_t>(erased)->s_ == "hallo");
  }
  {
    x_t a{"hallo"};
    value_base vb(a);
    REQUIRE(reconcrete_cast<x_t>(vb.get_lifetime_holder())->s_ == "hallo");
  }
}