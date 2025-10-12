#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/clone.hpp>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/virtual_typed.hpp>
#include <virtual_void/runtime/meta_data.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;

const double M_PI = 3.14;

struct position {
  float x, y;
};

namespace {
VV_INTERFACE(any_drawable, (VV_CONST_METHOD(void, draw, position)))

VV_INTERFACE_(any_shape, any_drawable,
              (VV_CONST_METHOD(int, count_sides), VV_CONST_METHOD(double, area),
               VV_CONST_METHOD(double, perimeter)))

VV_INTERFACE_(any_callabel_shape, any_shape,
              (VV_CONST_OP(std::string, 1, (), std::string const&)))
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

using shape_vv = any_shape<shared_const>;

using any_shape_v = any_shape<const_observer>;

using full_shape_observer = any_shape<mutable_observer>;

template <>
struct any_drawable_v_table_map<circle> {
  auto draw(circle const* x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_any_drawable_v_table_map At "
              << p.x << " " << p.y << std::endl;
  }
};

template <>
struct any_shape_v_table_map<circle>
    : any_shape_default_v_table_map<circle const> {
  auto draw(circle const* x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_any_shape_v_table_map At "
              << p.x << " " << p.y << std::endl;
  }
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
void print_shape(const any_callabel_shape<const_observer> s) {
  print_shape_(s);
  std::cout << s("Shape type = ") << std::endl;
}
void print_shape_f(const full_shape_observer s) { print_shape_(s); }

// using shape_double_base_error = any_shape<
// const_observer, bases<
// any_shape, any_shape > >; //should not compile! void
// should_not_compile(shape_double_base_error s) {}//should not compile!

TEST_CASE("dynamic v_table const_observer") {
  using namespace virtual_void;
  using namespace virtual_void::runtime;

  circle c{12.3};
  square s{32};
  rectangle r{12, 9};
  regular_polygon p{4, 32};

  std::cout << "print_shape *******************************" << std::endl;

  print_shape(c);
  print_shape(s);
  print_shape(r);
  print_shape(p);

  using erased_const_observer = const_observer;
  static_assert(std::is_base_of_v<interface::base<erased_const_observer>,
                                  any_callabel_shape<const_observer>>);
  static_assert(
      std::is_base_of_v<any_shape_v, any_callabel_shape<const_observer>>);
  static_assert(
      std::derived_from<any_callabel_shape<const_observer>, any_shape_v>);
  any_callabel_shape<const_observer> any_callabel_shape_onst_observer_circle1{
      circle{33.3}};
  any_callabel_shape<const_observer> any_callabel_shape_onst_observer_circle2{
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

  //    base< void* > base_v =  any_callabel_shape_onst_observer_circle1; ->
  //    downcast_to may not compile!
  virtual_void::interface::base<const_observer> base_shape =
      any_callabel_shape_onst_observer_circle1;
  virtual_void::interface::base<const_observer> base_shapeX =
      any_callabel_shape_onst_observer_circle2;

  REQUIRE(is_derived_from<any_callabel_shape<const_observer>>(base_shape));
  REQUIRE(is_derived_from<any_callabel_shape<const_observer>>(base_shape));
  REQUIRE(is_derived_from<any_callabel_shape<const_observer>>(
      any_callabel_shape_onst_observer_circle2));
  REQUIRE(is_derived_from<any_callabel_shape<const_observer>>(
      any_callabel_shape_onst_observer_circle2));
  static_assert(
      std::derived_from<any_callabel_shape<const_observer>, any_shape_v>);
  static_assert(
      std::derived_from<any_callabel_shape<const_observer>, any_shape_v>);
  REQUIRE(
      virtual_void::interface::downcast_to<any_callabel_shape<const_observer>>(
          base_shape));
  REQUIRE(
      virtual_void::interface::downcast_to<any_callabel_shape<const_observer>>(
          any_callabel_shape_onst_observer_circle2));
  {
    any_callabel_shape<const_observer> upcasted_shape =
        virtual_void::interface::unchecked_downcast_to<
            any_callabel_shape<const_observer>>(base_shape);
    print_shape(upcasted_shape);
  }

  any_shape_v shape_circle_base = any_callabel_shape_onst_observer_circle1;
  {
    any_callabel_shape<const_observer> any_shape_is_circle =
        virtual_void::interface::unchecked_downcast_to<
            any_callabel_shape<const_observer>>(shape_circle_base);
    print_shape(any_shape_is_circle);
  }
  {
    auto any_shape_is_circle = virtual_void::interface::downcast_to<
        any_callabel_shape<const_observer>>(shape_circle_base);
    REQUIRE(any_shape_is_circle);
    print_shape(*any_shape_is_circle);
  }

  print_shape_f(full_shape_observer{p});
}

TEST_CASE("dynamic interface shared_const") {
  auto c = std::make_shared<circle>(12.3);
  auto s = std::make_shared<square>(32);
  auto r = std::make_shared<rectangle>(12, 9);
  auto p = std::make_shared<regular_polygon>(4, 32);
  std::cout << "print_shape_vv ********************************" << std::endl;

  using typed_circle_shape_sc_no_meta =
      interface::virtual_typed<circle, any_shape<shared_const>>;
  typed_circle_shape_sc_no_meta sc_typed{c};
  auto& c1 = sc_typed;
  REQUIRE_THAT(c1->perimeter(), WithinAbs(77.2, 77.3));
  static_assert(
      std::same_as<typed_circle_shape_sc_no_meta::erased_data_t, shared_const>);
  static_assert(interface::is_virtual_typed<decltype(sc_typed)>);
  shape_vv circle_shape_vv{sc_typed};
  auto unerased_circle = unerase_cast<circle const>(circle_shape_vv);
  REQUIRE_THAT(unerased_circle->perimeter(), WithinAbs(77.2, 77.3));
  auto x = circle_shape_vv;
  {
    auto perimeter = circle_shape_vv.perimeter();
    REQUIRE_THAT(perimeter, WithinAbs(77.2, 77.3));
  }
  print_shape_vv(sc_typed);
  print_shape_vv(circle_shape_vv);
  print_shape_vv(c);
  print_shape_vv(s);
  print_shape_vv(r);
  print_shape_vv(p);
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
