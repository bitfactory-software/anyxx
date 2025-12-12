#include <bit_factory/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <variant>
#include <vector>

#include "double_dispatch_result.hpp"

namespace {

struct cat;
struct dog;
struct man;
struct creature;

struct creature_visitor {  // NOLINT
  virtual ~creature_visitor() = default;
  virtual encounter_result visit(creature const* l, cat const* r) const = 0;
  virtual encounter_result visit(creature const* l, dog const* r) const = 0;
  virtual encounter_result visit(creature const* l, man const* r) const = 0;
};

struct creature {  // NOLINT
  virtual ~creature() = default;
  [[nodiscard]] virtual std::type_info const* name() const = 0;
  [[nodiscard]] virtual creature_visitor const& get_encounter_visitor()
      const = 0;
  [[nodiscard]] virtual encounter_result apply(creature_visitor const& visitor,
                                               creature const* other) const = 0;
};
using creatures_t = std::vector<std::shared_ptr<creature>>;

struct cat final : creature {
  [[nodiscard]] std::type_info const* name() const override {
    return &typeid(*this);
  }
  [[nodiscard]] creature_visitor const& get_encounter_visitor()
      const override;
  [[nodiscard]] encounter_result apply(
      creature_visitor const& visitor, creature const* other) const override {
    return visitor.visit(other, this);
  }
};

struct dog final : creature {
  [[nodiscard]] std::type_info const* name() const override {
    return &typeid(*this);
  }
  [[nodiscard]] creature_visitor const& get_encounter_visitor()
      const override;
  [[nodiscard]] encounter_result apply(
      creature_visitor const& visitor, creature const* other) const override {
    return visitor.visit(other, this);
  }
};

struct man final : creature {
  [[nodiscard]] std::type_info const* name() const override {
    return &typeid(*this);
  }
  [[nodiscard]] creature_visitor const& get_encounter_visitor()
      const override;
  [[nodiscard]] encounter_result apply(
      creature_visitor const& visitor, creature const* other) const override {
    return visitor.visit(other, this);
  }
};

struct cat_encounter_visitor final : creature_visitor {
  [[nodiscard]] encounter_result visit(creature const* l,
                                       cat const* r) const override {
    return {l->name(), encounter_action::strolls_with, r->name()};
  }
  [[nodiscard]] encounter_result visit(creature const* l,
                                       dog const* r) const override {
    return {l->name(), encounter_action::hisses_at, r->name()};
  }
  [[nodiscard]] encounter_result visit(creature const* l,
                                       man const* r) const override {
    return {l->name(), encounter_action::nestle_to, r->name()};
  }
};
[[nodiscard]] creature_visitor const& cat::get_encounter_visitor() const {
  static cat_encounter_visitor vsitor;
  return vsitor;
}

struct dog_encounter_visitor final : creature_visitor {
  [[nodiscard]] encounter_result visit(creature const* l,
                                       cat const* r) const override {
    return {l->name(), encounter_action::chases, r->name()};
  }
  [[nodiscard]] encounter_result visit(creature const* l,
                                       dog const* r) const override {
    return {l->name(), encounter_action::snoops_at, r->name()};
  }
  [[nodiscard]] encounter_result visit(creature const* l,
                                       man const* r) const override {
    return {l->name(), encounter_action::nestle_to, r->name()};
  }
};
[[nodiscard]] creature_visitor const& dog::get_encounter_visitor() const {
  static dog_encounter_visitor vsitor;
  return vsitor;
}

struct man_encounter_visitor final : creature_visitor {
  [[nodiscard]] encounter_result visit(creature const* l,
                                       cat const* r) const override {
    return {l->name(), encounter_action::strokes, r->name()};
  }
  [[nodiscard]] encounter_result visit(creature const* l,
                                       dog const* r) const override {
    return {l->name(), encounter_action::strokes, r->name()};
  }
  [[nodiscard]] encounter_result visit(creature const* l,
                                       man const* r) const override {
    return {l->name(), encounter_action::shakes_hands_with, r->name()};
  }
};
[[nodiscard]] creature_visitor const& man::get_encounter_visitor() const {
  static man_encounter_visitor vsitor;
  return vsitor;
}

[[nodiscard]] encounter_result encounter(creature const* c1,
                                         creature const* c2) {
  return c2->apply(c1->get_encounter_visitor(), c1);
}

[[nodiscard]] auto apply_encounters(creatures_t const& creatures) {
  std::vector<encounter_result> result;
  result.reserve(9);
  for (auto&& c1 : creatures)
    for (auto&& c2 : creatures) result.push_back(encounter(c1.get(), c2.get()));
  return result;
}
}  // namespace

TEST_CASE("30b_Animals virtual functions") {
  creatures_t creatures{std::make_shared<dog>(), std::make_shared<cat>(),
                        std::make_shared<man>()};
  show("30b_Animals virtual functions", apply_encounters(creatures));

  CHECK(encounter(creatures[0].get(), creatures[0].get()) ==
        encounter_result{&typeid(dog), encounter_action::snoops_at,
                         &typeid(dog)});
  CHECK(encounter(creatures[0].get(), creatures[1].get()) ==
        encounter_result{&typeid(dog), encounter_action::chases, &typeid(cat)});
  CHECK(encounter(creatures[0].get(), creatures[2].get()) ==
        encounter_result{&typeid(dog), encounter_action::nestle_to,
                         &typeid(man)});
  CHECK(encounter(creatures[1].get(), creatures[0].get()) ==
        encounter_result{&typeid(cat), encounter_action::hisses_at,
                         &typeid(dog)});
  CHECK(encounter(creatures[1].get(), creatures[1].get()) ==
        encounter_result{&typeid(cat), encounter_action::strolls_with,
                         &typeid(cat)});
  CHECK(encounter(creatures[1].get(), creatures[2].get()) ==
        encounter_result{&typeid(cat), encounter_action::nestle_to,
                         &typeid(man)});
  CHECK(
      encounter(creatures[2].get(), creatures[0].get()) ==
      encounter_result{&typeid(man), encounter_action::strokes, &typeid(dog)});
  CHECK(
      encounter(creatures[2].get(), creatures[1].get()) ==
      encounter_result{&typeid(man), encounter_action::strokes, &typeid(cat)});
  CHECK(encounter(creatures[2].get(), creatures[2].get()) ==
        encounter_result{&typeid(man), encounter_action::shakes_hands_with,
                         &typeid(man)});

#ifndef _DEBUG
  BENCHMARK("30a_Animals virtual functions") {
    return apply_encounters(creatures);
  };
#endif  // !_DEBUG
}
