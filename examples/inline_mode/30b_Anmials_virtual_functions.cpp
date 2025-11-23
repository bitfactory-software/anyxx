#include <anyxx/anyxx.hpp>
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

struct creature_visitor {
  virtual encounter_result visit_cat(creature const* l,
                                     creature const* r) const = 0;
  virtual encounter_result visit_dog(creature const* l,
                                     creature const* r) const = 0;
  virtual encounter_result visit_man(creature const* l,
                                     creature const* r) const = 0;
};
using creature_visitor_ptr = std::unique_ptr<creature_visitor>;

struct creature {
  virtual ~creature() = default;
  virtual std::type_info const* name() const = 0;
  virtual std::unique_ptr<creature_visitor> get_encounter_visitor() const = 0;
  virtual encounter_result apply(creature_visitor const* visitor,
                                 creature const* other) const = 0;
};
using creatures_t = std::vector<std::shared_ptr<creature>>;

struct cat : creature {
  std::type_info const* name() const override { return &typeid(*this); }
  virtual creature_visitor_ptr get_encounter_visitor() const override;
  virtual encounter_result apply(creature_visitor const* visitor,
                                 creature const* other) const override {
    return visitor->visit_cat(other, this);
  }
};

struct dog : creature {
  std::type_info const* name() const override { return &typeid(*this); }
  virtual creature_visitor_ptr get_encounter_visitor() const override;
  virtual encounter_result apply(creature_visitor const* visitor,
                                 creature const* other) const override {
    return visitor->visit_dog(other, this);
  }
};

struct man : creature {
  std::type_info const* name() const override { return &typeid(*this); }
  virtual creature_visitor_ptr get_encounter_visitor() const override;
  virtual encounter_result apply(creature_visitor const* visitor,
                                 creature const* other) const override {
    return visitor->visit_man(other, this);
  }
};

struct cat_encounter_visitor : creature_visitor {
  encounter_result visit_cat(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::strolls_with, r->name()};
  }
  encounter_result visit_dog(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::hisses_at, r->name()};
  }
  encounter_result visit_man(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::nestle_to, r->name()};
  }
};
creature_visitor_ptr cat::get_encounter_visitor() const {
  return std::make_unique<cat_encounter_visitor>();
}

struct dog_encounter_visitor : creature_visitor {
  encounter_result visit_cat(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::chases, r->name()};
  }
  encounter_result visit_dog(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::snoops_at, r->name()};
  }
  encounter_result visit_man(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::nestle_to, r->name()};
  }
};
creature_visitor_ptr dog::get_encounter_visitor() const {
  return std::make_unique<dog_encounter_visitor>();
}

struct man_encounter_visitor : creature_visitor {
  encounter_result visit_cat(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::strokes, r->name()};
  }
  encounter_result visit_dog(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::strokes, r->name()};
  }
  encounter_result visit_man(creature const* l,
                             creature const* r) const override {
    return {l->name(), encounter_action::shakes_hands_with, r->name()};
  }
};
creature_visitor_ptr man::get_encounter_visitor() const {
  return std::make_unique<man_encounter_visitor>();
}

encounter_result encounter(creature const* c1, creature const* c2) {
  auto visitor = c1->get_encounter_visitor();
  return c2->apply(visitor.get(), c1);
}

auto apply_encounters(creatures_t const& creatures) {
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
  show( "30b_Animals virtual functions", apply_encounters(creatures));

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
