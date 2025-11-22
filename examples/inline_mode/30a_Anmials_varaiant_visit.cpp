#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace {

struct cat {
  static std::string name() { return "Cat"; }
};
struct dog {
  static std::string name() { return "Dog"; }
};
struct man {
  static std::string name() { return "Man"; }
};

// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

using creature = std::variant<cat, dog, man>;
using creatures_t = std::vector<creature>;

std::string encounter(creature const& a, creature const& b) {
  return std::visit(overloaded{[](cat const& c, dog const& d) {
                                 return c.name() + " hisses at " + d.name();
                               },
                               [](dog const& d, cat const& c) {
                                 return d.name() + " chases " + c.name();
                               },
                               [](dog const& d, dog const& c) {
                                 return d.name() + " snoops at " + c.name();
                               },
                               [](cat const& d, cat const& c) {
                                 return d.name() + " strolls with " + c.name();
                               },
                               [](man const& a, man const& b) {
                                 return a.name() + " shakes hands with " +
                                        b.name();
                               },
                               [](man const& a, auto const& b) {
                                 return a.name() + " strokes " + b.name();
                               },
                               [](auto const& a, man const& b) {
                                 return a.name() + " nestle to " + b.name();
                               }},
                    a, b);
}

auto apply_encounters(creatures_t const& creatures) {
  std::stringstream output;
  for (auto&& c1 : creatures)
    for (auto&& c2 : creatures) output << encounter(c1, c2) << "\n";
  return output.str();
}
}  // namespace

TEST_CASE("30a_Animals variant visit") {
  creatures_t creatures{dog{}, cat{}, man{}};
  std::cout << apply_encounters(creatures);

#ifndef _DEBUG
  BENCHMARK("30a_Animals variant visit") {
    return apply_encounters(creatures);
  };
#endif  // !_DEBUG
}
