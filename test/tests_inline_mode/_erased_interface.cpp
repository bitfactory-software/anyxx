#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <anypp/clone_erased_data.hpp>
#include <anypp/observer.hpp>
#include <anypp/shared_const.hpp>
#include <anypp/unique.hpp>
#include <anypp/any_meta_class.hpp>
#include <anypp/typed_any.hpp>
#include <anypp/meta_data.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void;

const double M_PI = 3.14;

struct position {
  float x, y;
};

namespace {
VV_ANY(any_drawable, (VV_CONST_METHOD(void, draw, position)))

VV_ANY_(any_shape, any_drawable,
              (VV_CONST_METHOD(int, count_sides), VV_CONST_METHOD(double, area),
               VV_CONST_METHOD(double, perimeter)))

VV_ANY_(any_callable_shape, any_shape,
              (VV_CONST_OP(std::string, (), std::string const&)))
}  // namespace

struct circle {
  double radius = 10;
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

template <>
struct any_drawable_v_table_map<circle> {
  auto draw(circle const* x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_any_drawable_v_table_map At "
              << p.x << " " << p.y << std::endl;
  }
};

void print_any_shape_const_observer(const any_shape<const_observer> s) {
  s.draw({4.0, 5.0});
  std::cout << "Shape Number Of Sides: " << s.count_sides() << std::endl;
  std::cout << "Shape Perimeter: " << s.perimeter() << std::endl;
  std::cout << "Shape Area: " << s.area() << std::endl;
}
void print_any_callable_shape_const_observer(const any_callable_shape<const_observer> s) {
  print_any_shape_const_observer(s);
  std::cout << s("Shape type = ") << std::endl;
}

// using shape_double_base_error = any_shape<
// const_observer, bases<
// any_shape, any_shape > >; //should not compile! void
// should_not_compile(shape_double_base_error s) {}//should not compile!

TEST_CASE("dynamic v_table const_observer") {
  using namespace virtual_void;
  using namespace virtual_void;

  circle c{12.3};
  square s{32};
  rectangle r{12, 9};
  regular_polygon p{4, 32};

  std::cout << "print_shape *******************************" << std::endl;

  print_any_callable_shape_const_observer(c);
  print_any_callable_shape_const_observer(s);
  print_any_callable_shape_const_observer(r);
  print_any_callable_shape_const_observer(p);

  using erased_const_observer = const_observer;
  static_assert(std::is_base_of_v<any_base<erased_const_observer>,
                                  any_callable_shape<const_observer>>);
  static_assert(
      std::is_base_of_v<any_shape<const_observer>, any_callable_shape<const_observer>>);
  static_assert(
      std::derived_from<any_callable_shape<const_observer>, any_shape<const_observer>>);
  any_callable_shape<const_observer> any_callable_shape_onst_observer_circle1{
      circle{33.3}};
  any_callable_shape<const_observer> any_callable_shape_onst_observer_circle2{
      circle{33.3}};

  const_observer o1 = erased<const_observer>(c);
  const_observer o2 = o1;

  {
    using any_drawable_const_observer = any_drawable<const_observer>;
    any_drawable_const_observer sb1;
    any_drawable_const_observer sb2{c};
    sb1 = sb2;
  }
  {
    using any_drawable_mutable_observer = any_drawable<mutable_observer>;
    any_drawable_mutable_observer sb1;
    any_drawable_mutable_observer sb2{c};
    sb1 = sb2;
  }
  {
    using any_drawable_mutable_observer = any_drawable<mutable_observer>;
    any_drawable_mutable_observer sb1{c};
    any_drawable_mutable_observer sb2{std::move(sb1)};
  }

  //    any_base< void* > base_v =  any_callable_shape_onst_observer_circle1; ->
  //    downcast_to may not compile!
  virtual_void::any_base<const_observer> base_shape =
      any_callable_shape_onst_observer_circle1;
  virtual_void::any_base<const_observer> base_shapeX =
      any_callable_shape_onst_observer_circle2;

  REQUIRE(is_derived_from<any_callable_shape<const_observer>>(base_shape));
  REQUIRE(is_derived_from<any_callable_shape<const_observer>>(base_shape));
  REQUIRE(is_derived_from<any_callable_shape<const_observer>>(
      any_callable_shape_onst_observer_circle2));
  REQUIRE(is_derived_from<any_callable_shape<const_observer>>(
      any_callable_shape_onst_observer_circle2));
  static_assert(
      std::derived_from<any_callable_shape<const_observer>, any_shape<const_observer>>);
  static_assert(
      std::derived_from<any_callable_shape<const_observer>, any_shape<const_observer>>);
  REQUIRE(
      virtual_void::downcast_to<any_callable_shape<const_observer>>(
          base_shape));
  REQUIRE(
      virtual_void::downcast_to<any_callable_shape<const_observer>>(
          any_callable_shape_onst_observer_circle2));
  {
    any_callable_shape<const_observer> upcasted_shape =
        virtual_void::unchecked_downcast_to<
            any_callable_shape<const_observer>>(base_shape);
    print_any_callable_shape_const_observer(upcasted_shape);
  }

  any_shape<const_observer> shape_circle_base = any_callable_shape_onst_observer_circle1;
  {
    any_callable_shape<const_observer> any_shape_is_circle =
        virtual_void::unchecked_downcast_to<
            any_callable_shape<const_observer>>(shape_circle_base);
    print_any_callable_shape_const_observer(any_shape_is_circle);
  }
  {
    auto any_shape_is_circle = virtual_void::downcast_to<
        any_callable_shape<const_observer>>(shape_circle_base);
    REQUIRE(any_shape_is_circle);
    print_any_callable_shape_const_observer(*any_shape_is_circle);
  }

  print_any_shape_const_observer(any_shape<mutable_observer>{p});
}

TEST_CASE("dynamic interface shared_const") {
  auto c = std::make_shared<circle>(12.3);
  auto s = std::make_shared<square>(32);
  auto r = std::make_shared<rectangle>(12, 9);
  auto p = std::make_shared<regular_polygon>(4, 32);
  std::cout << "print_shape_vv ********************************" << std::endl;

  using typed_circle_shape_shared_const =
      typed_any<circle, any_shape<shared_const>>;
  typed_circle_shape_shared_const sc_typed{c};
  auto& c1 = sc_typed;
  REQUIRE_THAT(c1->perimeter(), WithinAbs(77.2, 77.3));
  static_assert(
      std::same_as<typed_circle_shape_shared_const::erased_data_t, shared_const>);
  static_assert(is_typed_any<decltype(sc_typed)>);
  any_shape<shared_const> circle_shape_vv{sc_typed};
  auto unerased_circle = unerase_cast<circle const>(circle_shape_vv);
  REQUIRE_THAT(unerased_circle->perimeter(), WithinAbs(77.2, 77.3));
  auto x = circle_shape_vv;
  {
    auto perimeter = circle_shape_vv.perimeter();
    REQUIRE_THAT(perimeter, WithinAbs(77.2, 77.3));
  }
  print_any_shape_const_observer(sc_typed);
  print_any_shape_const_observer(circle_shape_vv);
  print_any_callable_shape_const_observer(*c);
  print_any_callable_shape_const_observer(*s);
  print_any_callable_shape_const_observer(*r);
  print_any_callable_shape_const_observer(*p);
}

namespace {
void print_any_shape_co(any_shape<const_observer> s) { s.draw({1, 2}); }

}  // namespace
TEST_CASE("dynamic interface unique") {
  auto c = std::make_unique<circle>(12.3);

  using shape_unique = any_shape<unique>;
  shape_unique s1{std::move(c)};

  REQUIRE_THAT(s1.perimeter(), WithinAbs(77.2, 77.3));
  auto unerased_circle = unerase_cast<circle const>(s1);
  REQUIRE_THAT(unerased_circle->perimeter(), WithinAbs(77.2, 77.3));

  static_assert(borrowable_from<const_observer, unique>);
  print_any_shape_co(s1);
}
