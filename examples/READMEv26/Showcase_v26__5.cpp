#define ANYXX_26
#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/factory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

namespace {

namespace ayx = anyxx;

struct circle {
  double radius = 0.0;
  [[nodiscard]] double area() const { return radius * radius * 3.14; }
};
struct square {
  [[nodiscard]] double area() const { return edge_length * edge_length; }
  double edge_length = 0.0;
};

template <typename Self>
struct figure : ayx::dynamic_copyable<Self> {
    double area() const;
};
template <typename Self>
struct serializable : ayx::dynamic_copyable<Self> {
    void serialize(std::ostream& os) const;
};

ayx::factory<ayx::any<serializable>, std::string, std::istream&> deserialize;

[[nodiscard]] ayx::any<figure> deserialize_any_figure(std::istream& archive) {
  std::string type;
  archive >> type;
  return ayx::move_to<ayx::any<figure>>(deserialize(type, archive));
}

template<>
struct serializable<circle> {
  static void serialize(circle const& self, std::ostream& os) {
    os << "circle " << self.radius << " ";
  };
};
auto __ = deserialize.register_("circle", [](std::istream& archive) -> circle {
  circle c;
  archive >> c.radius;
  return c;
});
anyxx::castable_to<circle, figure> __;
anyxx::castable_to<circle, serializable> __;

template<>
struct serializable<square> {
  static void serialize(square const& self, std::ostream& os) {
    os << "square " << self.edge_length << " ";
  };
};
auto __ = deserialize.register_("square", [](std::istream& archive) {
  square s;
  archive >> s.edge_length;
  return s;
});
anyxx::castable_to<square, figure> __;
anyxx::castable_to<square, serializable> __;

void areas(std::stringstream& os,
           std::vector<ayx::any<figure>> const& figures) {
  std::string sep;
  for (auto const& f : figures) os << std::exchange(sep, ", ") << f.area();
}

TEST_CASE("Showcase5") {
  std::stringstream archive{": circle 1 : square 2 : circle 3 : circle 4 end"};
  std::vector<ayx::any<figure>> figures;
  std::string more;
  for (archive >> more; more != "end"; archive >> more)
    figures.push_back(deserialize_any_figure(archive));

  std::stringstream ss;
  areas(ss, figures);
  CHECK(ss.str() == "3.14, 4, 28.26, 50.24");

  std::stringstream serialized;
  for (auto const& f : figures) {
    serialized << ": ";
    ayx::borrow_as<ayx::any_<ayx::cref, serializable>>(f)->serialize(serialized);
  }
  serialized << "end";

  CHECK(serialized.str() == archive.str());
}
};  // namespace showcase5
