#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/shared_const.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void;
using namespace virtual_void;

namespace {

const double M_PI = 3.14;

struct position {
  float x = 0.0, y = 0.0;
};
std::ostream& operator<<(std::ostream& os, position p) {
  os << p.x << ", " << p.y;
  return os;
}

struct circle {
  double radius = 10;
  void draw(std::ostream& os, position p) const {
    os << "Circle at: " << p << "\n";
  }
  int count_sides() const { return 1; }
  double area() const { return radius * radius * M_PI; }
  double circumference() const { return radius * 2.0f * M_PI; }
  double perimeter() const { return circumference(); }
};
struct square {
  int w;
  void draw(std::ostream& os, position p) const {
    os << "Square at: " << p << "\n";
  }
  int count_sides() const { return 4; }
  double area() const { return w * w; }
  double perimeter() const { return w * 4; }
};
struct rectangle {
  int w, h;
  void draw(std::ostream& os, position p) const {
    os << "Rectangle at: " << p << "\n";
  }
  int count_sides() const { return 4; }
  double area() const { return w * h; }
  double perimeter() const { return w + w + h + h; }
};
struct regular_polygon {
  int sides;
  double side_length;
  void draw(std::ostream& os, position p) const {
    os << "Polygon at: " << p << "\n";
  }
  int count_sides() const { return sides; }
  double apothem() const { return (side_length / 2) / std::tan(M_PI / sides); }
  double radius() const { return (side_length / 2) / std::sin(M_PI / sides); }
  double perimeter() const { return sides * side_length; }
  double area() const { return (perimeter() * apothem()) / 2; }
};

VV_INTERFACE(any_drawable,
             (VV_CONST_METHOD(void, draw, std::ostream&, position)))

VV_INTERFACE_(any_shape, any_drawable,
              (VV_CONST_METHOD(int, count_sides), VV_CONST_METHOD(double, area),
               VV_CONST_METHOD(double, perimeter)))

template <>
struct any_drawable_v_table_map<std::string> {
  auto draw(std::string const* x, std::ostream& os, position p) const {
    os << *x << " at: " << p << "\n";
  }
};

void draw(std::ostream& os,
          std::vector<any_drawable<shared_const>> const& any_drawables) {
  float x = 0.0, y = 0.0;
  for (auto const& any_drawable : any_drawables)
    any_drawable.draw(os, {x++, y++});
}

void process(
    std::ostream& os,
    std::vector<any_drawable<shared_const>> const& any_drawables,
    std::function<void(std::ostream&, any_drawable<shared_const> const&)> const&
        command) {
  for (auto const& any_drawable : any_drawables) command(os, any_drawable);
}

}  // namespace

TEST_CASE("example 1/1") {
  std::stringstream os;
  draw(os, {std::make_shared<circle>(12.3), std::make_shared<square>(32),
            std::make_shared<rectangle>(12, 9),
            std::make_shared<regular_polygon>(4, 32),
            std::make_shared<std::string>("hello world")});
  CHECK(os.str() ==
        "Circle at: 0, 0\n"
        "Square at: 1, 1\n"
        "Rectangle at: 2, 2\n"
        "Polygon at: 3, 3\n"
        "hello world at: 4, 4\n");
}

TEST_CASE("example 1/2 upcast, downcast") {
  std::vector<any_shape<shared_const>> any_shapes{
      std::make_shared<circle>(12.3),
      std::make_shared<square>(32),
      std::make_shared<rectangle>(12, 9),
      std::make_shared<regular_polygon>(4, 32),
  };
  std::vector<any_drawable<shared_const>> any_drawables;
  any_drawables.append_range(any_shapes);
  any_drawables.push_back(std::make_shared<std::string>("hello world"));

  std::stringstream os1;
  draw(os1, any_drawables);
  CHECK(os1.str() ==
        "Circle at: 0, 0\n"
        "Square at: 1, 1\n"
        "Rectangle at: 2, 2\n"
        "Polygon at: 3, 3\n"
        "hello world at: 4, 4\n");

  std::stringstream os2;
  process(os2, any_drawables,
          [](std::ostream& os, any_drawable<shared_const> const& drawable) {
            downcast_to<any_shape<shared_const>>(drawable).and_then(
                [&](any_shape<shared_const> const& shape) {
                  shape.draw(os, {});
                  os << "...area: " << shape.area() << "\n";
                  return std::optional{shape};
                });
          });
  CHECK(os2.str() ==
        "Circle at: 0, 0\n"
        "...area: 475.051\n"
        "Square at: 0, 0\n"
        "...area: 1024\n"
        "Rectangle at: 0, 0\n"
        "...area: 108\n"
        "Polygon at: 0, 0\n"
        "...area: 1024.82\n");
}
