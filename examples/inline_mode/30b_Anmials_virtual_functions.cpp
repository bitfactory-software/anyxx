#include <anyxx/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace {

struct cat;
struct dog;
struct man;
struct creature;

struct encounter_visitor {
  virtual std::string encounter_cat(creature const* self,
                                    creature const* x) const = 0;
  virtual std::string encounter_dog(creature const* self,
                                    creature const* x) const = 0;
  virtual std::string encounter_man(creature const* self,
                                    creature const* x) const = 0;
};
using encounter_visitor_ptr = std::unique_ptr<encounter_visitor>;

struct creature {
  virtual ~creature() = default;
  virtual std::string name() const = 0;
  virtual std::unique_ptr<encounter_visitor> get_encounter_visitor() const = 0;
  virtual std::string apply(encounter_visitor const* visitor,
                           creature const* other) const = 0;
};
using creatures_t = std::vector<std::shared_ptr<creature>>;

struct cat : creature {
  std::string name() const override { return "Cat"; }
  virtual std::unique_ptr<encounter_visitor> get_encounter_visitor()
      const override;
  virtual std::string apply(encounter_visitor const* visitor,
                           creature const* other) const override {
    return visitor->encounter_cat(this, other);
  }
};

struct dog : creature {
  std::string name() const override { return "Dog"; }
  virtual std::unique_ptr<encounter_visitor> get_encounter_visitor()
      const override;
  virtual std::string apply(encounter_visitor const* visitor,
                           creature const* other) const override {
    return visitor->encounter_dog(this, other);
  }
};

struct man : creature {
  std::string name() const override { return "Man"; }
  virtual std::unique_ptr<encounter_visitor> get_encounter_visitor()
      const override;
  virtual std::string apply(encounter_visitor const* visitor,
                           creature const* other) const override {
    return visitor->encounter_man(this, other);
  }
};

struct cat_encounter_visitor : encounter_visitor {
  std::string encounter_cat(creature const* self,
                            creature const* x) const override {
    return self->name() + " strolls with " + x->name();
  }
  std::string encounter_dog(creature const* self,
                        creature const* x) const override {
    return self->name() + " hisses at " + x->name();
  }
  std::string encounter_man(creature const* self,
                        creature const* x) const override {
    return self->name() + " nestle to " + x->name();
  }
};
std::unique_ptr<encounter_visitor> cat::get_encounter_visitor() const {
  return std::make_unique<cat_encounter_visitor>();
}

struct dog_encounter_visitor : encounter_visitor {
  std::string encounter_cat(creature const* self,
                            creature const* x) const override {
    return self->name() + " chases " + x->name();
  }
  std::string encounter_dog(creature const* self,
                            creature const* x) const override {
    return self->name() + " snoops at " + x->name();
  }
  std::string encounter_man(creature const* self,
                            creature const* x) const override {
    return self->name() + " nestle to " + x->name();
  }
};
std::unique_ptr<encounter_visitor> dog::get_encounter_visitor() const {
  return std::make_unique<cat_encounter_visitor>();
}

struct man_encounter_visitor : encounter_visitor {
  std::string encounter_cat(creature const* self,
                            creature const* x) const override {
    return self->name() + " strokes " + x->name();
  }
  std::string encounter_dog(creature const* self,
                            creature const* x) const override {
    return self->name() + " strokes " + x->name();
  }
  std::string encounter_man(creature const* self,
                            creature const* x) const override {
    return self->name() + " shakes hands with " + x->name();
  }
};
std::unique_ptr<encounter_visitor> man::get_encounter_visitor() const {
  return std::make_unique<man_encounter_visitor>();
}

std::string encounter(creature const* c1, creature const* c2) {
  auto visitor = c1->get_encounter_visitor();
  return c1->apply(visitor.get(), c2);
}

auto apply_encounters(creatures_t const& creatures) {
  std::stringstream output;
  for (auto&& c1 : creatures)
    for (auto&& c2 : creatures) output << encounter(c1.get(), c2.get()) << "\n";
  return output.str();
}
}  // namespace

TEST_CASE("30a_Animals virtual functions") {
  creatures_t creatures{std::make_shared<dog>(), std::make_shared<cat>(),
                        std::make_shared<man>()};
  std::cout << apply_encounters(creatures);

#ifndef _DEBUG
  BENCHMARK("30a_Animals virtual functions") {
    return apply_encounters(creatures);
  };
#endif  // !_DEBUG
}
