#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/typed_any.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <iostream>
#include <string>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

using namespace anyxx;

namespace smoke_test {
struct position {
  float x, y;
};

TRAIT(drawable, anyxx::dynamic_copyable, (FN(void, draw, (position), const)))
TRAIT(shape, drawable,
      (FN(int, count_sides, (), const), FN(double, area, (), const),
       FN(double, perimeter, (), const)))
TRAIT(callable_shape, shape,
      (OP_(std::string, (), (std::string const&), const)))

struct circle {
  double radius = 10;
  void draw(position p) const {
    std::cout << " A Circle Is Recorded At " << p.x << " " << p.y << "\n";
  }
  [[nodiscard]] int count_sides() const { return 1; }
  [[nodiscard]] double area() const { return radius * radius * 3.14; }
  [[nodiscard]] double circumference() const { return radius * 2.0 * 3.14; }
  [[nodiscard]] double perimeter() const { return circumference(); }
  [[nodiscard]] std::string operator()(const std::string& x) const {
    return x + "circle";
  }
};
struct square {
  int w;
  void draw(position p) const {
    std::cout << " A Square Is Recorded At " << p.x << " " << p.y << "\n";
  }
  [[nodiscard]] int count_sides() const { return 4; }
  [[nodiscard]] double area() const { return w * w; }
  [[nodiscard]] double perimeter() const { return w * 4; }
  [[nodiscard]] std::string operator()(const std::string& x) const {
    return x + "square";
  }
};
struct rectangle {
  int w, h;
  void draw(position p) const {
    std::cout << " A Rectangle Is Recorded At " << p.x << " " << p.y << "\n";
  }
  [[nodiscard]] int count_sides() const { return 4; }
  [[nodiscard]] double area() const { return w * h; }
  [[nodiscard]] double perimeter() const { return w + w + h + h; }
  [[nodiscard]] std::string operator()(const std::string& x) const {
    return x + "rectangle";
  }
};
struct regular_polygon {
  int sides;
  double side_length;
  void draw(position p) const {
    std::cout << " A Polygon Is Recorded At " << p.x << ", " << p.y << "\n";
  }
  [[nodiscard]] int count_sides() const { return sides; }
  [[nodiscard]] double apothem() const {
    return (side_length / 2) / std::tan(3.14 / sides);
  }
  [[nodiscard]] double radius() const {
    return (side_length / 2) / std::sin(3.14 / sides);
  }
  [[nodiscard]] double perimeter() const { return sides * side_length; }
  [[nodiscard]] double area() const { return (perimeter() * apothem()) / 2; }
  [[nodiscard]] std::string operator()(const std::string& x) const {
    return x + "regular_polygon";
  }
};

MODEL_MAP(circle, drawable) {
  auto draw([[maybe_unused]] circle const& x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_any_drawable_model_map At "
              << p.x << " " << p.y << "\n";
  };  // namespace smoke_test
};

void print_any_shape_const_observer(any_<cref, shape> const& s) {
  s.draw(position{4.0, 5.0});  // NOLINT
  std::cout << "Shape Number Of Sides: " << s.count_sides() << "\n";
  std::cout << "Shape Perimeter: " << s.perimeter() << "\n";
  std::cout << "Shape Area: " << s.area() << "\n";
}
void print_any_callable_shape_const_observer(
    const any_<cref, callable_shape> s) {
  print_any_shape_const_observer(s);
  std::cout << s("Shape type = ") << "\n";
}

// using shape_double_base_error = any_shape<
// cref, bases<
// any_shape, any_shape > >; //should not compile! void
// should_not_compile(shape_double_base_error s) {}//should not compile!

}  // namespace smoke_test

using namespace smoke_test;

TEST_CASE("dynamic v_table cref") {
  circle c{12.3};
  square s{32};
  rectangle r{12, 9};  // NOLINT
  regular_polygon p{.sides = 4, .side_length = 32};

  std::cout << "print_shape *******************************" << "\n";

  print_any_callable_shape_const_observer(c);
  print_any_callable_shape_const_observer(s);
  print_any_callable_shape_const_observer(r);
  print_any_callable_shape_const_observer(p);

  auto a_circle = circle{33.3};
  any_<cref, callable_shape> any_callable_shape_onst_observer_circle1{a_circle};
  any_<cref, callable_shape> any_callable_shape_onst_observer_circle2{a_circle};

  auto o1 = erased<cref>(c);
  [[maybe_unused]] cref o2 = o1;

  {
    using any_drawable_mutable_observer = any_<mutref, drawable>;
    any_drawable_mutable_observer sb1{c};
    any_drawable_mutable_observer sb2{std::move(sb1)};
  }

  //    any_< void* > base_v =  any_callable_shape_onst_observer_circle1; ->
  //    downcast_to may not compile!
  [[maybe_unused]] anyxx::any_<cref, dynamic_copyable> base_shape =
      any_callable_shape_onst_observer_circle1;
  [[maybe_unused]] anyxx::any_<cref, dynamic_copyable> base_shapeX =
      any_callable_shape_onst_observer_circle2;

  REQUIRE(is_derived_from<any_<cref, callable_shape>>(base_shape));
  REQUIRE(is_derived_from<any_<cref, callable_shape>>(base_shape));
  REQUIRE(is_derived_from<any_<cref, callable_shape>>(
      any_callable_shape_onst_observer_circle2));
  REQUIRE(is_derived_from<any_<cref, callable_shape>>(
      any_callable_shape_onst_observer_circle2));
  static_assert(std::derived_from<any_<cref, callable_shape>::v_table_t,
                                  any_<cref, shape>::v_table_t>);
  static_assert(std::derived_from<any_<cref, callable_shape>::v_table_t,
                                  any_<cref, shape>::v_table_t>);
  REQUIRE(anyxx::downcast_to<any_<cref, callable_shape>>(base_shape));
  REQUIRE(anyxx::downcast_to<any_<cref, callable_shape>>(
      any_callable_shape_onst_observer_circle2));

  if (auto downcasted_shape =
          anyxx::downcast_to<any_<cref, callable_shape>>(base_shape)) {
    print_any_callable_shape_const_observer(*downcasted_shape);
  } else {
    FAIL("downcast_to failed");
  }

  any_<cref, shape> shape_circle_base = any_callable_shape_onst_observer_circle1;
  {
    auto any_shape_is_circle =
        anyxx::unchecked_downcast_to<any_<cref, callable_shape>>(
            shape_circle_base);
    print_any_callable_shape_const_observer(any_shape_is_circle);
  }
  {
    auto any_shape_is_circle =
        anyxx::downcast_to<any_<cref, callable_shape>>(shape_circle_base);
    REQUIRE(any_shape_is_circle);
    print_any_callable_shape_const_observer(*any_shape_is_circle);
  }

  print_any_shape_const_observer(any_<mutref, shape>{p});
}

TEST_CASE("dynamic any_ shared") {
  auto c = std::make_shared<circle>(12.3);
  auto s = std::make_shared<square>(32);
  auto r = std::make_shared<rectangle>(12, 9);
  any_<shared, callable_shape> p{regular_polygon(4, 32)};
  std::cout << "print_shape_vv ********************************\n";

  using typed_circle_shape_shared_const = typed_any<circle, any_<shared, shape>>;
  typed_circle_shape_shared_const sc_typed{c};
  auto& c1 = sc_typed;
  REQUIRE_THAT(c1->perimeter(), Catch::Matchers::WithinAbs(77.25, 0.1));
  static_assert(std::same_as<typed_circle_shape_shared_const::proxy_t, shared>);
  static_assert(is_typed_any<decltype(sc_typed)>);
  any_<shared, shape> circle_shape_vv{sc_typed};
  decltype(auto) unerased_circle = any_cast<circle const>(circle_shape_vv);
  REQUIRE_THAT(unerased_circle.perimeter(),
               Catch::Matchers::WithinAbs(77.25, 0.1));
  auto x = circle_shape_vv;  // NOLINT
  {
    auto perimeter = circle_shape_vv.perimeter();
    REQUIRE_THAT(perimeter, Catch::Matchers::WithinAbs(77.25, 0.1));
  }
  print_any_shape_const_observer(
      static_cast<typed_circle_shape_shared_const::any_t&>(sc_typed));
  print_any_shape_const_observer(circle_shape_vv);
  print_any_callable_shape_const_observer(*c);
  print_any_callable_shape_const_observer(*s);
  print_any_callable_shape_const_observer(*r);
  print_any_callable_shape_const_observer(p);
}
