#include <algorithm>
#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/factory.hpp>
#include <catch2/catch_test_macros.hpp>
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
  return std::ranges::fold_right(
      shapes, 0.0,
      [](const any<shape>& s, double acc) { return acc + s.area(); });
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
    out.push_back(deserialize.construct(type, archive));
  }
  return out;
}

}  // namespace db

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
ANY_REGISTER_MODEL(application::circle, db::serializable);
ANY_REGISTER_MODEL(application::circle, geometry::shape);

auto __ = db::deserialize.register_("square", [](std::istream& ar) {
  application::square s;
  ar >> s.edge_length;
  return s;
});
ANY_REGISTER_MODEL(application::square, db::serializable);
ANY_REGISTER_MODEL(application::square, geometry::shape);

}  // namespace

TEST_CASE("talk_23_8_factory_and_crosscast") {
  using namespace anyxx;

  std::stringstream archive{": circle 1 : square 2 : circle 3 : circle 4 end"};
  auto deserialized = db::deserialize_from(archive);

  auto shapes = deserialized | std::views::transform([](const auto& d) {
                  return *borrow_as<any<geometry::shape>>(d);
                });
  static_assert(std::same_as<std::ranges::range_value_t<decltype(shapes)>,
                             any<geometry::shape>>);
  CHECK(geometry::sum_area(shapes) == 85.64);
}
