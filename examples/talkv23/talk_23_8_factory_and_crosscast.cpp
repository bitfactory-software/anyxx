#include <algorithm>
#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/factory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <format>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

using namespace anyxx;

namespace {

template <typename R, typename V>
concept range_of =
    std::ranges::range<R> && std::same_as<std::ranges::range_value_t<R>, V>;

namespace geometry {

TRAIT(shape, dynamic_castable, (FN(double, area, (), const)))

template <range_of<any<shape>> Shapes>
double sum_area(const Shapes& shapes) {
  return std::ranges::fold_left(
      shapes, 0.0,
      [](double acc, const any<shape>& s) { return acc + s.area(); });
}
}  // namespace geometry

namespace db {

TRAIT(serializable, dynamic_moveable,
    (FN(void, serialize, (std::ostream&), const)))

factory<any<serializable>, std::string, std::istream&> deserialize;

auto deserialize_from(std::istream& archive) {
  std::vector<any<serializable>> out;
  std::string more;
  for (archive >> more; more != "end"; archive >> more) {
    std::string type;
    archive >> type;
    out.push_back(deserialize(type, archive));
  }
  return out;
}

}  // namespace db

TEST_CASE("talk_23_8_factory_and_crosscast") {
  using namespace anyxx;

  std::stringstream archive{": circle 1 : square 2 : circle 3 : circle 4 end"};
  auto deserialized = db::deserialize_from(archive);

  auto shapes = deserialized | std::views::transform([](const auto& d) {
                  return *borrow_as<any<geometry::shape>>(d);
                });
  static_assert(std::same_as<std::ranges::range_value_t<decltype(shapes)>,
                             any<geometry::shape>>);
  CHECK_THAT(geometry::sum_area(shapes), Catch::Matchers::WithinAbs(85.64, 0.01));
}

namespace application {
struct circle {
    double radius = 0.0;
    double area() const { return radius * radius * 3.14; }
    void serialize(std::ostream& os) const { os << "circle " << radius << " "; };
};
struct square {
    double edge_length = 0.0;
    double area() const { return edge_length * edge_length; }
    void serialize(std::ostream& os) const {
        os << "square " << edge_length << " ";
    };
};
}  // namespace application

auto __ = db::deserialize.register_("circle", [](std::istream& ar) {
    application::circle c;
    ar >> c.radius;
    return c;
});
enable_dynamic<application::circle, db::serializable> __;
enable_dynamic<application::circle, geometry::shape> __;

auto __ = db::deserialize.register_("square", [](std::istream& ar) {
    application::square s;
    ar >> s.edge_length;
    return s;
});
enable_dynamic<application::square, db::serializable> __;
enable_dynamic<application::square, geometry::shape> __;

}  // namespace

