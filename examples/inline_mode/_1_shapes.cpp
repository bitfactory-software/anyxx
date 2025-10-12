#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;
using namespace virtual_void::interface;

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
  void draw(position p) const { std::cout << "Circle at: " << p << std::endl; }
  int count_sides() const { return 1; }
  double area() const { return radius * radius * M_PI; }
  double circumference() const { return radius * 2.0f * M_PI; }
  double perimeter() const { return circumference(); }
};
struct square {
  int w;
  void draw(position p) const { std::cout << "Square at: " << p << std::endl; }
  int count_sides() const { return 4; }
  double area() const { return w * w; }
  double perimeter() const { return w * 4; }
};
struct rectangle {
  int w, h;
  void draw(position p) const {
    std::cout << "Rectangle at: " << p << std::endl;
  }
  int count_sides() const { return 4; }
  double area() const { return w * h; }
  double perimeter() const { return w + w + h + h; }
};
struct regular_polygon {
  int sides;
  double side_length;
  void draw(position p) const { std::cout << "Polygon at: " << p << std::endl; }
  int count_sides() const { return sides; }
  double apothem() const { return (side_length / 2) / std::tan(M_PI / sides); }
  double radius() const { return (side_length / 2) / std::sin(M_PI / sides); }
  double perimeter() const { return sides * side_length; }
  double area() const { return (perimeter() * apothem()) / 2; }
};

VV_INTERFACE(any_drawable, (VV_CONST_METHOD(void, draw, position)))

VV_INTERFACE_(any_shape, any_drawable,
              (VV_CONST_METHOD(int, count_sides), VV_CONST_METHOD(double, area),
               VV_CONST_METHOD(double, perimeter)))

template <>
struct any_drawable_v_table_map<std::string> {
  auto draw(std::string const* x, position p) const {
    std::cout << *x << " at: " << p << std::endl;
  }
};

void draw(std::vector<any_drawable<shared_const>> const& any_drawables) {
  float x = 0.0, y = 0.0;
  std::cout << "+++" << std::endl;
  for (auto const& any_drawable : any_drawables) any_drawable.draw({x++, y++});
  std::cout << "--" << std::endl;
}

void process(
    std::vector<any_drawable<shared_const>> const& any_drawables,
    std::function<void(any_drawable<shared_const> const&)> const& command) {
  std::cout << "+++ process" << std::endl;
  for (auto const& any_drawable : any_drawables) command(any_drawable);
  std::cout << "--" << std::endl;
}

}  // namespace

TEST_CASE("example 1/1") {
  draw({std::make_shared<circle>(12.3), std::make_shared<square>(32),
        std::make_shared<rectangle>(12, 9),
        std::make_shared<regular_polygon>(4, 32),
        std::make_shared<std::string>("hello world")});
}

TEST_CASE("example 1/2 upcast, downcast") {
  std::vector<any_drawable<shared_const>> any_drawables{
      std::make_shared<std::string>("hello world")};

  std::vector<any_shape<shared_const>> any_shapes{
      std::make_shared<circle>(12.3),
      std::make_shared<square>(32),
      std::make_shared<rectangle>(12, 9),
      std::make_shared<regular_polygon>(4, 32),
  };
  CHECK(is_derived_from<any_shape<shared_const>>(any_shapes.back()));

  any_drawables.append_range(any_shapes);
  draw(any_drawables);

  process(any_drawables, [](any_drawable<shared_const> const& drawable) {
    downcast_to<any_shape<shared_const>>(drawable).and_then(
        [](any_shape<shared_const> const& shape) {
          shape.draw({});
          std::cout << "...area: " << shape.area() << std::endl;
          return std::optional{shape};
        });
  });
}
