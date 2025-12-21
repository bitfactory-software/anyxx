#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <iostream>
#include <string>

using namespace anyxx;

namespace smoke_test {
struct position {
  float x, y;
};

ANY(any_drawable, (ANY_METHOD(void, draw, (position), const)), , )

ANY_(any_shape, any_drawable,
     (ANY_METHOD(int, count_sides, (), const),
      ANY_METHOD(double, area, (), const),
      ANY_METHOD(double, perimeter, (), const)), , )

ANY_(any_callable_shape, any_shape,
     (ANY_OP(std::string, (), (std::string const&), const)), , )

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

ANY_MODEL_MAP((circle), any_drawable) {
  auto draw([[maybe_unused]] circle const& x, position p) const {
    std::cout << " A Circle Is Recorded VIA circle_any_drawable_model_map At "
              << p.x << " " << p.y << "\n";
  };  // namespace smoke_test
};

void print_any_shape_const_observer(const any_shape<const_observer> s) {
  s.draw({4.0, 5.0});  // NOLINT
  std::cout << "Shape Number Of Sides: " << s.count_sides() << "\n";
  std::cout << "Shape Perimeter: " << s.perimeter() << "\n";
  std::cout << "Shape Area: " << s.area() << "\n";
}
void print_any_callable_shape_const_observer(
    const any_callable_shape<const_observer> s) {
  print_any_shape_const_observer(s);
  std::cout << s("Shape type = ") << "\n";
}

// using shape_double_base_error = any_shape<
// const_observer, bases<
// any_shape, any_shape > >; //should not compile! void
// should_not_compile(shape_double_base_error s) {}//should not compile!

}  // namespace smoke_test

using namespace smoke_test;

TEST_CASE("dynamic v_table const_observer") {
  circle c{12.3};
  square s{32};
  rectangle r{12, 9};  // NOLINT
  regular_polygon p{.sides = 4, .side_length = 32};

  std::cout << "print_shape *******************************" << "\n";

  print_any_callable_shape_const_observer(c);
  print_any_callable_shape_const_observer(s);
  print_any_callable_shape_const_observer(r);
  print_any_callable_shape_const_observer(p);

  using erased_const_observer = const_observer;
  static_assert(std::is_base_of_v<any_base<erased_const_observer>,
                                  any_callable_shape<const_observer>>);
  static_assert(std::is_base_of_v<any_shape<const_observer>,
                                  any_callable_shape<const_observer>>);
  static_assert(std::derived_from<any_callable_shape<const_observer>,
                                  any_shape<const_observer>>);
  auto a_circle = circle{33.3};
  any_callable_shape<const_observer> any_callable_shape_onst_observer_circle1{
      a_circle};
  any_callable_shape<const_observer> any_callable_shape_onst_observer_circle2{
      a_circle};

  auto o1 = erased<const_observer>(c);
  [[maybe_unused]] const_observer o2 = o1;

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
  [[maybe_unused]] anyxx::any_base<const_observer> base_shape =
      any_callable_shape_onst_observer_circle1;
  [[maybe_unused]] anyxx::any_base<const_observer> base_shapeX =
      any_callable_shape_onst_observer_circle2;

  REQUIRE(is_derived_from<any_callable_shape<const_observer>>(base_shape));
  REQUIRE(is_derived_from<any_callable_shape<const_observer>>(base_shape));
  REQUIRE(is_derived_from<any_callable_shape<const_observer>>(
      any_callable_shape_onst_observer_circle2));
  REQUIRE(is_derived_from<any_callable_shape<const_observer>>(
      any_callable_shape_onst_observer_circle2));
  static_assert(std::derived_from<any_callable_shape<const_observer>,
                                  any_shape<const_observer>>);
  static_assert(std::derived_from<any_callable_shape<const_observer>,
                                  any_shape<const_observer>>);
  REQUIRE(anyxx::downcast_to<any_callable_shape<const_observer>>(base_shape));
  REQUIRE(anyxx::downcast_to<any_callable_shape<const_observer>>(
      any_callable_shape_onst_observer_circle2));

  if (auto downcasted_shape =
          anyxx::downcast_to<any_callable_shape<const_observer>>(base_shape)) {
    print_any_callable_shape_const_observer(*downcasted_shape);
  } else {
    FAIL("downcast_to failed");
  }

  any_shape<const_observer> shape_circle_base =
      any_callable_shape_onst_observer_circle1;
  {
    auto any_shape_is_circle =
        anyxx::unchecked_downcast_to<any_callable_shape<const_observer>>(
            shape_circle_base);
    print_any_callable_shape_const_observer(any_shape_is_circle);
  }
  {
    auto any_shape_is_circle =
        anyxx::downcast_to<any_callable_shape<const_observer>>(
            shape_circle_base);
    REQUIRE(any_shape_is_circle);
    print_any_callable_shape_const_observer(*any_shape_is_circle);
  }

  print_any_shape_const_observer(any_shape<mutable_observer>{p});
}

TEST_CASE("dynamic any shared_const") {
  auto c = std::make_shared<circle>(12.3);
  auto s = std::make_shared<square>(32);
  auto r = std::make_shared<rectangle>(12, 9);
  auto p = std::make_shared<regular_polygon>(4, 32);
  std::cout << "print_shape_vv ********************************\n";

  using typed_circle_shape_shared_const =
      typed_any<circle, any_shape, shared_const>;
  typed_circle_shape_shared_const sc_typed{c};
  auto& c1 = sc_typed;
  REQUIRE_THAT(c1->perimeter(), Catch::Matchers::WithinAbs(77.2, 77.3));
  static_assert(std::same_as<typed_circle_shape_shared_const::erased_data_t,
                             shared_const>);
  static_assert(is_typed_any<decltype(sc_typed)>);
  any_shape<shared_const> circle_shape_vv{sc_typed};
  auto unerased_circle = unerase_cast<circle const>(circle_shape_vv);
  REQUIRE_THAT(unerased_circle->perimeter(),
               Catch::Matchers::WithinAbs(77.2, 77.3));
  auto x = circle_shape_vv;  // NOLINT
  {
    auto perimeter = circle_shape_vv.perimeter();
    REQUIRE_THAT(perimeter, Catch::Matchers::WithinAbs(77.2, 77.3));
  }
  print_any_shape_const_observer(sc_typed);
  print_any_shape_const_observer(circle_shape_vv);
  print_any_callable_shape_const_observer(*c);
  print_any_callable_shape_const_observer(*s);
  print_any_callable_shape_const_observer(*r);
  print_any_callable_shape_const_observer(*p);
}

namespace {
void print_any_shape_co(any_shape<const_observer> s) {
  s.draw({.x = 1, .y = 2});
}

}  // namespace
TEST_CASE("dynamic any unique") {
  auto c = std::make_unique<circle>(12.3);

  using shape_unique = any_shape<unique>;
  shape_unique s1{std::move(c)};

  REQUIRE_THAT(s1.perimeter(), Catch::Matchers::WithinAbs(77.2, 77.3));
  auto unerased_circle = unerase_cast<circle const>(s1);
  REQUIRE_THAT(unerased_circle->perimeter(),
               Catch::Matchers::WithinAbs(77.2, 77.3));

  static_assert(borrowable_from<const_observer, unique>);
  print_any_shape_co(s1);
}
