#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/dispatch.hpp>
#include <bit_factory/anyxx/ext/dispatch_table.hpp>
#include <catch2/catch_test_macros.hpp>
#include <compare>
#include <sstream>
#include <string>
#include <vector>

namespace {
// [slide]
struct circle {
  [[nodiscard]] std::string name() const { return "circle"; }
};
struct square {
  [[nodiscard]] std::string name() const { return "square"; }
};

TRAIT_EX(figure, anyxx::dynamic_copyable, (FN(std::string, name, (), const)), ,
         , (HAS_OPEN_DISPATCH), ())

anyxx::dispatch<std::partial_ordering(anyxx::virtual_<anyxx::any<figure>>,
                                      anyxx::virtual_<anyxx::any<figure>>)>
    compare_edges;

[[nodiscard]] std::partial_ordering operator<=>(anyxx::any<figure> const& l,
                                                anyxx::any<figure> const& r) {
  return compare_edges(l, r);
}

auto __ = compare_edges.define<circle, circle>(
    [](auto const&, auto const&) { return std::partial_ordering::equivalent; });
auto __ = compare_edges.define<circle, square>(
    [](auto const&, auto const&) { return std::partial_ordering::less; });
auto __ = compare_edges.define<square, square>(
    [](auto const&, auto const&) { return std::partial_ordering::equivalent; });
auto __ = compare_edges.define<square, circle>(
    [](auto const&, auto const&) { return std::partial_ordering::greater; });

void compare_each(std::stringstream& os,
                  std::vector<anyxx::any<figure>> const& figures) {
  std::string sep;
  for (auto const& l : figures)
    for (auto const& r : figures) {
      os << std::exchange(sep, ", ") << l.name() << " ";
      if (auto c = l <=> r; c == std::partial_ordering::equivalent)
        os << "==";
      else if (c == std::partial_ordering::less)
        os << "<";
      else if (c == std::partial_ordering::greater)
        os << ">";
      else
        os << "?";
      os << " " << r.name();
    }
}

TEST_CASE("Showcase3") {
  std::stringstream ss;
  compare_each(ss, {circle{}, square{}});
  CHECK(ss.str() ==
        "circle == circle, circle < square, square > circle, square == square");
}
// [slide]
};  // namespace
