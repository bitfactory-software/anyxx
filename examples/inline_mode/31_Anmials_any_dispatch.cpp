#include <anyxx/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace {}

ANY_HAS_DISPATCH(, any_creature)
namespace {
ANY(any_creature, (ANY_METHOD(std::string, name, (), const)))
}  // namespace

namespace {

struct cat {
  static std::string name() { return "cat"; }
};
struct dog {
  static std::string name() { return "dog"; }
};
struct man {
  static std::string name() { return "man"; }
};

auto encounter = anyxx::dispatch<std::string(
    anyxx::virtual_<any_creature<anyxx::const_observer>>,
    anyxx::virtual_<any_creature<anyxx::const_observer>>)>{};

auto __ = encounter.define<cat, dog>([](auto const& c, auto const& d) {
  return c.name() + " hisses at " + d.name();
});
auto __ = encounter.define<dog, cat>([](auto const& d, auto const& c) {
  return d.name() + " chases " + c.name();
});
auto __ = encounter.define<dog, dog>([](auto const& d, auto const& c) {
  return d.name() + " snoops at " + c.name();
});
auto __ = encounter.define<cat, cat>([](auto const& a, auto const& b) {
  return a.name() + " strolls with " + b.name();
});
auto __ = encounter.define<man, man>([](auto const& a, auto const& b) {
  return a.name() + " shakes hands with " + b.name();
});
auto __ = encounter.define<man, any_creature<anyxx::const_observer>>(
    [](auto const& a, auto const& b) {
      return a.name() + " strokes " + b.name();
    });
auto __ = encounter.define<any_creature<anyxx::const_observer>, man>(
    [](auto const& a, man const& b) {
      return a.name() + " nestle to " + b.name();
    });

using creatures_t = std::vector<any_creature<anyxx::value>>;
auto apply_encounters(creatures_t const& creatures) {
  std::stringstream output;
  for (auto&& c1 : creatures)
    for (auto&& c2 : creatures) output << encounter(c1, c2) << "\n";
  return output.str();
}
}  // namespace

TEST_CASE("31_Animals any dispatch") {
  creatures_t creatures{dog{}, cat{}, man{}};
  std::cout << "+++31_Animals any dispatch\n";
  std::cout << apply_encounters(creatures);
  std::cout << "---31_Animals any dispatch\n";

  CHECK(encounter(creatures[0], creatures[0]) == "dog snoops at dog");
  CHECK(encounter(creatures[0], creatures[1]) == "dog chases cat");
  CHECK(encounter(creatures[0], creatures[2]) == "dog nestle to man");
  CHECK(encounter(creatures[1], creatures[0]) == "cat hisses at dog");
  CHECK(encounter(creatures[1], creatures[1]) == "cat strolls with cat");
  CHECK(encounter(creatures[1], creatures[2]) == "cat nestle to man");
  CHECK(encounter(creatures[2], creatures[0]) == "man strokes dog");
  CHECK(encounter(creatures[2], creatures[1]) == "man strokes cat");
  CHECK(encounter(creatures[2], creatures[2]) == "man shakes hands with man");

#ifndef _DEBUG
  BENCHMARK("30a_Animals any dispatch") { return apply_encounters(creatures); };
#endif  // !_DEBUG
}
