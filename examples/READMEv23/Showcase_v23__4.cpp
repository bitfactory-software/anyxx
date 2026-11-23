#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/dispatch.hpp>
#include <bit_factory/anyxx/ext/dispatch_table.hpp>
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace {

namespace ayx = anyxx;

struct circle {
  [[nodiscard]] std::string name() const { return "circle"; }
};
struct square {
  [[nodiscard]] std::string name() const { return "square"; }
};

TRAIT_EX(figure, anyxx::dynamic_copyable, (FN(std::string, name, (), const)), ,
         , (HAS_OPEN_DISPATCH), ())
using any_figure = anyxx::any<figure>;

ayx::dispatch<std::string(ayx::virtual_<any_figure>)> latin, italian;

auto __ = latin.define<circle>([](auto const&) { return "orbis"; });
auto __ = latin.define<square>([](auto const&) { return "quadratum"; });
auto __ = italian.define<circle>([](auto const&) { return "cerchio"; });
auto __ = italian.define<square>([](auto const&) { return "quadrato"; });

void translate(std::stringstream& os, std::vector<any_figure> const& figures) {
  std::string sep;
  for (auto const& f : figures)
    os << std::exchange(sep, "; ") << f.name() << ": latin = " << latin(f)
       << ", italian = " << italian(f);
}

TEST_CASE("Showcase4") {
  std::stringstream ss;
  translate(ss, {circle{}, square{}});
  CHECK(ss.str() ==
        "circle: latin = orbis, italian = cerchio; square: latin = quadratum, "
        "italian = quadrato");
}
}  // namespace
