#include <bit_factory/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <typeinfo>
#include <variant>
#include <vector>

#include "double_dispatch_result.hpp"

namespace {}

ANY_HAS_DISPATCH(, any_creature)
namespace {
ANY(any_creature, (ANY_METHOD(std::type_info const*, name, (), const)))
}  // namespace

namespace {

struct cat {
  auto name() const { return &typeid(*this); }
};
struct dog {
  auto name() const { return &typeid(*this); }
};
struct man {
  auto name() const { return &typeid(*this); }
};
auto encounter = anyxx::dispatch<encounter_result(
    anyxx::virtual_<any_creature<anyxx::const_observer>>,
    anyxx::virtual_<any_creature<anyxx::const_observer>>)>{};

auto __ = encounter.define<cat, dog>(
    [](auto const& l, auto const& r) -> encounter_result {
      return {l.name(), encounter_action::hisses_at, r.name()};
    });
auto __ = encounter.define<dog, cat>(
    [](auto const& l, auto const& r) -> encounter_result {
      return {l.name(), encounter_action::chases, r.name()};
    });
auto __ = encounter.define<dog, dog>(
    [](auto const& l, auto const& r) -> encounter_result {
      return {l.name(), encounter_action::snoops_at, r.name()};
    });
auto __ = encounter.define<cat, cat>(
    [](auto const& l, auto const& r) -> encounter_result {
      return {l.name(), encounter_action::strolls_with, r.name()};
    });
auto __ = encounter.define<man, man>(
    [](auto const& l, auto const& r) -> encounter_result {
      return {l.name(), encounter_action::shakes_hands_with, r.name()};
    });
auto __ = encounter.define<man, any_creature<anyxx::const_observer>>(
    [](auto const& l, auto const& r) -> encounter_result {
      return {l.name(), encounter_action::strokes, r.name()};
    });
auto __ = encounter.define<any_creature<anyxx::const_observer>, man>(
    [](auto const& l, man const& r) -> encounter_result {
      return {l.name(), encounter_action::nestle_to, r.name()};
    });

using creatures_t = std::vector<any_creature<anyxx::value>>;
auto apply_encounters(creatures_t const& creatures) {
  std::vector<encounter_result> result;
  result.reserve(9);
  for (auto&& c1 : creatures)
    for (auto&& c2 : creatures) result.push_back(encounter(c1, c2));
  return result;
}
}  // namespace

TEST_CASE("31_Animals any dispatch") {
  creatures_t creatures{dog{}, cat{}, man{}};
  show("31_Animals any dispatch", apply_encounters(creatures));

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
  std::cout << "Ensure 'target_compile_options(examples_inline_mode PRIVATE /Ob2)' is used!\n";
  BENCHMARK("30a_Animals any dispatch") { return apply_encounters(creatures); };
#endif  // !_DEBUG
}
