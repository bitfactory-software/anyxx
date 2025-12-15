#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <typeinfo>
#include <variant>
#include <vector>
#include <algorithm>

#include "double_dispatch_result.hpp"

namespace {

struct cat {
  [[nodiscard]]auto name() const { return &typeid(*this); }
};
struct dog {
  [[nodiscard]]auto name() const { return &typeid(*this); }
};
struct man {
  [[nodiscard]]auto name() const { return &typeid(*this); }
};

// helper type for the visitor #4
template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

using creature = std::variant<cat, dog, man>;
using creatures_t = std::vector<creature>;

[[nodiscard]]encounter_result encounter(creature const& a, creature const& b) {
  return std::visit(
      overloaded{[](cat const& l, dog const& r) -> encounter_result {
                   return {l.name(), encounter_action::hisses_at, r.name()};
                 },
                 [](dog const& l, cat const& r) -> encounter_result {
                   return {l.name(), encounter_action::chases, r.name()};
                 },
                 [](dog const& l, dog const& r) -> encounter_result {
                   return {l.name(), encounter_action::snoops_at, r.name()};
                 },
                 [](cat const& l, cat const& r) -> encounter_result {
                   return {l.name(), encounter_action::strolls_with, r.name()};
                 },
                 [](man const& l, man const& r) -> encounter_result {
                   return {l.name(), encounter_action::shakes_hands_with,
                           r.name()};
                 },
                 [](man const& l, auto const& r) -> encounter_result {
                   return {l.name(), encounter_action::strokes, r.name()};
                 },
                 [](auto const& l, man const& r) -> encounter_result {
                   return {l.name(), encounter_action::nestle_to, r.name()};
                 }},
      a, b);
}

// cppcheck-suppress-begin [useStlAlgorithm]
auto apply_encounters(creatures_t const& creatures) {
  std::vector<encounter_result> result;
  result.reserve(9);
  for (auto&& c1 : creatures)
    for (auto&& c2 : creatures) result.push_back(encounter(c1, c2));
  return result;
}
// cppcheck-suppress-end [useStlAlgorithm]
}  // namespace

TEST_CASE("30a_Animals variant visit") {
  creatures_t creatures{dog{}, cat{}, man{}};
  show("+30a_Animals variant visit", apply_encounters(creatures));

  CHECK(encounter(creatures[0], creatures[0]) ==
        encounter_result{&typeid(dog), encounter_action::snoops_at,
                         &typeid(dog)});
  CHECK(encounter(creatures[0], creatures[1]) ==
        encounter_result{&typeid(dog), encounter_action::chases, &typeid(cat)});
  CHECK(encounter(creatures[0], creatures[2]) ==
        encounter_result{&typeid(dog), encounter_action::nestle_to,
                         &typeid(man)});
  CHECK(encounter(creatures[1], creatures[0]) ==
        encounter_result{&typeid(cat), encounter_action::hisses_at,
                         &typeid(dog)});
  CHECK(encounter(creatures[1], creatures[1]) ==
        encounter_result{&typeid(cat), encounter_action::strolls_with,
                         &typeid(cat)});
  CHECK(encounter(creatures[1], creatures[2]) ==
        encounter_result{&typeid(cat), encounter_action::nestle_to,
                         &typeid(man)});
  CHECK(
      encounter(creatures[2], creatures[0]) ==
      encounter_result{&typeid(man), encounter_action::strokes, &typeid(dog)});
  CHECK(
      encounter(creatures[2], creatures[1]) ==
      encounter_result{&typeid(man), encounter_action::strokes, &typeid(cat)});
  CHECK(encounter(creatures[2], creatures[2]) ==
        encounter_result{&typeid(man), encounter_action::shakes_hands_with,
                         &typeid(man)});
#ifndef _DEBUG
  BENCHMARK("30a_Animals variant visit") {
    return apply_encounters(creatures);
  };
#endif  // !_DEBUG
}
