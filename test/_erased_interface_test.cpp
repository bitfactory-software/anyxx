#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/clone.hpp>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/call_operator.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/virtual_typed.hpp>
#include <virtual_void/runtime/meta_data.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;

const double M_PI = 3.14;

struct position {
  float x, y;
};

namespace {
VV_INTERFACE(shape_base1, (VV_CONST_METHOD(void, draw, position)))

VV_INTERFACE_(shape_base, shape_base1, (VV_CONST_METHOD(int, count_sides)))

VV_INTERFACE_(shape_d_i, shape_base,
              (VV_CONST_METHOD(double, area),
               VV_CONST_METHOD(double, perimeter)))

VV_INTERFACE_(shape_i, shape_base1,
              (VV_CONST_METHOD(int, count_sides), VV_CONST_METHOD(double, area),
               VV_CONST_METHOD(double, perimeter)))
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

VV_RUNTIME_STATIC(circle)
VV_V_TABLE_INSTANCE(, circle, shape_base1)
VV_V_TABLE_INSTANCE(, circle, shape_base)
VV_V_TABLE_INSTANCE(, circle, shape_d_i)
VV_V_TABLE_INSTANCE(, circle, shape_i)

TEST_CASE("class is_a interface") {
  using namespace virtual_void;
  using namespace virtual_void::runtime;

  auto& circle_i_table = get_meta_data<circle>().get_i_table();
  REQUIRE(circle_i_table.size() >= 0);
  static_assert(
      std::same_as<shape_d_i_v_table::v_table_base_t, shape_base_v_table>);
  {
    circle c{};
    shape_d_i<data::mutable_observer> x{c};
    auto vtable1 = get_meta_data<circle>().get_v_table(
        typeid(shape_d_i<data::const_observer>::v_table_t));
    auto vtable2 = virtual_void::interface::get_v_table(x);
    REQUIRE(vtable1 == vtable2);
  }
}

using shape_vv = shape_i<data::shared_const>;

using shape_base_v = shape_base<data::const_observer>;

using shape =
    interface::call_operator<data::const_observer, std::string(std::string),
                             const_, shape_d_i>;
using shapeX = shape_d_i<data::const_observer>;
using shapeXX = shape_d_i<data::const_observer>;

using full_shape_observer = shape_i<data::mutable_observer>;

template <>
struct shape_base1_v_table_map<circle> {
  auto draw(circle const* x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_shape_base1_v_table_map At "
              << p.x << " " << p.y << std::endl;
  }
};

template <>
struct shape_i_v_table_map<circle> : shape_i_default_v_table_map<circle const> {
  auto draw(circle const* x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_shape_i_v_table_map At "
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
VV_RUNTIME_STATIC(square)
VV_V_TABLE_INSTANCE(, square, shape_base1)
VV_V_TABLE_INSTANCE(, square, shape_base)
VV_V_TABLE_INSTANCE(, square, shape_d_i)
VV_V_TABLE_INSTANCE(, square, shape_i)
VV_RUNTIME_STATIC(rectangle)
VV_V_TABLE_INSTANCE(, rectangle, shape_base1)
VV_V_TABLE_INSTANCE(, rectangle, shape_base)
VV_V_TABLE_INSTANCE(, rectangle, shape_d_i)
VV_V_TABLE_INSTANCE(, rectangle, shape_i)
VV_RUNTIME_STATIC(regular_polygon)
VV_V_TABLE_INSTANCE(, regular_polygon, shape_base1)
VV_V_TABLE_INSTANCE(, regular_polygon, shape_base)
VV_V_TABLE_INSTANCE(, regular_polygon, shape_d_i)
VV_V_TABLE_INSTANCE(, regular_polygon, shape_i)

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

// using shape_double_base_error = shape_d_i<
// data::const_observer, bases<
// shape_base, shape_base > >; //should not compile! void
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

  using erased_const_observer = data::const_observer;
  static_assert(
      std::is_base_of_v<interface::base<erased_const_observer>, shape>);
  static_assert(std::is_base_of_v<shape_base_v, shape>);
  static_assert(std::derived_from<shape, shape_base_v>);
  shape shape_circle{circle{33.3}};
  shapeX shape_circleX{circle{33.3}};

  data::const_observer o1 = data::erased<data::const_observer>(c);
  data::const_observer o2 = o1;

  {
    using shape_base1_const_observer = shape_base1<data::const_observer>;
    shape_base1_const_observer sb1;
    shape_base1_const_observer sb2{c};
    sb1 = sb2;
  }
  {
    using shape_base1_mutable_observer = shape_base1<data::mutable_observer>;
    shape_base1_mutable_observer sb1;
    shape_base1_mutable_observer sb2{c};
    sb1 = sb2;
  }
  {
    using shape_base1_mutable_observer = shape_base1<data::mutable_observer>;
    shape_base1_mutable_observer sb1{c};
    shape_base1_mutable_observer sb2{std::move(sb1)};
  }

  //    base< void* > base_v = shape_circle; ->
  //    downcast_to may not compile!
  virtual_void::interface::base<data::const_observer> base_shape = shape_circle;
  virtual_void::interface::base<data::const_observer> base_shapeX =
      shape_circleX;

  REQUIRE(is_derived_from<shape>(base_shape));
  REQUIRE(is_derived_from<shapeX>(base_shape));
  REQUIRE(!is_derived_from<shape>(shape_circleX));
  REQUIRE(is_derived_from<shapeX>(shape_circleX));
  static_assert(std::derived_from<shape, shape_base_v>);
  static_assert(std::derived_from<shapeX, shape_base_v>);
  REQUIRE(virtual_void::interface::downcast_to<shapeX>(base_shape));
  REQUIRE(!virtual_void::interface::downcast_to<shape>(shape_circleX));
  {
    shape upcasted_shape =
        virtual_void::interface::unchecked_downcast_to<shape>(base_shape);
    print_shape(upcasted_shape);
  }

  shape_base_v shape_circle_base = shape_circle;
  {
    shape shape_is_circle =
        virtual_void::interface::unchecked_downcast_to<shape>(
            shape_circle_base);
    print_shape(shape_is_circle);
  }
  {
    auto shape_is_circle =
        virtual_void::interface::downcast_to<shape>(shape_circle_base);
    REQUIRE(shape_is_circle);
    print_shape(*shape_is_circle);
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
      interface::virtual_typed<circle, shape_i<data::shared_const>>;
  typed_circle_shape_sc_no_meta sc_typed{c};
  auto& c1 = sc_typed;
  REQUIRE_THAT(c1->perimeter(), WithinAbs(77.2, 77.3));
  static_assert(std::same_as<typed_circle_shape_sc_no_meta::erased_data_t,
                             data::shared_const>);
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
void print_shape_i_co(shape_i<data::const_observer> s) { s.draw({1, 2}); }

}  // namespace
TEST_CASE("dynamic interface unique") {
  auto c = std::make_unique<circle>(12.3);

  using shape_unique = shape_i<data::unique>;
  shape_unique s1{std::move(c)};

  REQUIRE_THAT(s1.perimeter(), WithinAbs(77.2, 77.3));
  auto unerased_circle = unerase_cast<circle const>(s1);
  REQUIRE_THAT(unerased_circle->perimeter(), WithinAbs(77.2, 77.3));

  static_assert(data::borrowable_from<data::const_observer, data::unique>);
  print_shape_i_co(s1);
}

namespace {
struct x_t {
  std::string s_;
};
}  // namespace
VV_RUNTIME_STATIC(x_t)

TEST_CASE("base") {
  using namespace virtual_void;
  using namespace virtual_void;
  using value = data::shared_const;

  using value_base = interface::base<value>;

  {
    auto e = data::erased<value>(std::make_shared<x_t>("hallo"));
    REQUIRE(data::unchecked_unerase_cast<x_t>(e)->s_ == "hallo");
  }
  {
    value_base vb(std::make_shared<x_t>("hallo"));
    REQUIRE(unerase_cast<x_t>(vb)->s_ == "hallo");
    CHECK_THROWS_AS(unerase_cast<std::string>(vb), type_mismatch_error);
  }
}