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

template <typename T>
struct shape_base1_defaultmap {
  auto draw(T* x, position _sig) const {
    return x->draw(std::forward<decltype(_sig)>(_sig));
  }
};

template <typename T>
constexpr shape_base1_defaultmap<T> shape_base1_concept_map = {};

template <typename BASE_V_TABLE>
struct shape_base1interface : BASE_V_TABLE {
  using interface_base_t = BASE_V_TABLE;
  using void_t = interface_base_t::void_t;
  using v_table_t = shape_base1interface;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : interface_base_t::static_is_derived_from(from);
  }
  void (*draw)(void_t, position);
  template <typename UNERASE>
  shape_base1interface(UNERASE unerase)
      : interface_base_t(unerase), draw([](void_t _vp, position _sig) {
          return shape_base1_concept_map<typename UNERASE::type>.draw(
              (UNERASE{}(_vp)), std::forward<decltype(_sig)>(_sig));
        }) {
    virtual_void::erased::set_is_derived_from<v_table_t>(this);
  };
};
template <virtual_void::erased::is_erased_lifetime_holder LIFETIME_HOLDER>
struct shape_base1 : virtual_void::erased::base<LIFETIME_HOLDER> {
 public:
  using lifetime_holder_t = LIFETIME_HOLDER;
  using void_t = LIFETIME_HOLDER::void_t;
  using base_t = virtual_void::erased::base<LIFETIME_HOLDER>;
  using interface_base_t = base_t::interface_t;
  using interface_t = shape_base1interface<interface_base_t>;
  using query_interface_unique_t =
      shape_base1interface<virtual_void::erased::base<lifetime_holder_t>>;
  template <typename T>
  using is_already_base =
      std::conditional_t<std::is_same_v<T, query_interface_unique_t>,
                         std::true_type,
                         typename base_t::template is_already_base<T>>;
  static_assert(!base_t::is_already_base<query_interface_unique_t>::value,
                "An interface my only be once in instanciated for a facade");

 protected:
  using base_t::interface_impementation_;
  using base_t::lifetime_holder_;

 public:
  shape_base1(lifetime_holder_t lifetime_holder, interface_t* v_table)
      : base_t(std::move(lifetime_holder), v_table) {}
  template <typename CONSTRUCTED_WITH>
  shape_base1(CONSTRUCTED_WITH&& v)
    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, base_t>)
      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
    static interface_t imlpemented_interface{
        virtual_void::erased::unerase<LIFETIME_HOLDER, CONSTRUCTED_WITH>()};
    interface_impementation_ = &imlpemented_interface;
  }
  template <typename OTHER>
  shape_base1(const OTHER& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(other) {}
  void draw(position _sig)
    requires(!LIFETIME_HOLDER::is_const)
  {
    return static_cast<interface_t*>(interface_impementation_)
        ->draw(base_t::lifetime_holder_.data(),
               std::forward<decltype(_sig)>(_sig));
  }
  void draw(position _sig) const {
    return static_cast<interface_t*>(interface_impementation_)
        ->draw(base_t::lifetime_holder_.data(),
               std::forward<decltype(_sig)>(_sig));
  }
  shape_base1(const shape_base1&) = default;
  shape_base1(shape_base1&) = default;
  shape_base1(shape_base1&&) = default;

 protected:
  shape_base1() = default;
};

ERASED_INTERFACE_(shape_base, shape_base1, INTERFACE_METHOD(int, count_sides))

ERASED_INTERFACE_(shape_d_i, shape_base, INTERFACE_METHOD(double, area),
                  INTERFACE_METHOD(double, perimeter))

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

struct circle_shape_base1_concept_map {
  auto draw(circle const* x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_shape_base1_concept_map At "
              << p.x << " " << p.y << std::endl;
  }
};
template <>
constexpr auto shape_base1_concept_map<const circle> =
    circle_shape_base1_concept_map{};

struct circle_shape_i_concept_map : shape_i_defaultmap<circle const> {
  auto draw(circle const* x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_shape_i_concept_map At "
              << p.x << " " << p.y << std::endl;
  }
};
template <>
constexpr auto shape_i_concept_map<const circle> = circle_shape_i_concept_map{};

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
      std::is_same_v<decltype(sc), m_table::typed_shared_const<circle>>);
  static_assert(std::is_base_of_v<m_table::shared_const,
                                  m_table::typed_shared_const<circle>>);
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