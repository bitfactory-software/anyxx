#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/dispatch.hpp>
#include <bit_factory/anyxx/ext/dispatch_table.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <ranges>
#include <string>
#include <vector>

using namespace anyxx;

namespace {

TRAIT_EX(thing, anyxx::dynamic_copyable, 
    (FN(std::string, registry, (), const)), , , 
    (HAS_OPEN_DISPATCH), ())

dispatch<any<thing>(virtual_<any<thing>>, virtual_<any<thing>>)> collide;
any<thing> operator<=>(any<thing> const& l, any<thing> const& r) {
  return collide(l, r);
}
any<thing> run(std::vector<any<thing>> things) {
  auto winner = things.front();
  for (auto next : things | std::views::drop(1)) {
    std::print("{} <=> {} -> ", winner.registry(), next.registry());
    winner = winner <=> next;
    std::println("{}!", winner.registry());
  }
  return winner;
}

struct Asteroid {
  std::string name_;
  auto registry() const { return "Asteroid: " + name_; }
};
struct Spaceship {
  std::string name_;
  auto registry() const { return "Spaceship - " + name_; }
};

auto __ = collide.define<Asteroid, Spaceship>([](auto l, auto) {  //
  return l;
});
auto __ = collide.define<Asteroid, Asteroid>([](auto l, auto r) {  //
  return l.registry() > r.registry() ? l : r;
});
auto __ = collide.define<Spaceship, Spaceship>([](auto l, auto r) {  //
  return l.registry() < r.registry() ? l : r;
});
auto __ = collide.define<Spaceship, Asteroid>([](auto l, auto) {  //
  return l;
});
}  // namespace

TEST_CASE("talk_2310_basic_multi_dispatch") {
  auto winner = run({Asteroid{"Chicxulub"}, Spaceship{"NCC-1701"},
                     Spaceship{"USGS Powell"}, Asteroid{"Tunguska"}});
  CHECK(winner.registry() == "Asteroid: Tunguska");
}

namespace {
struct DeathStar {
  std::string n_;
  auto registry() const { return "DS" + n_; }
};
auto __ = collide.define<DeathStar, any<thing>>([](auto l, auto) {  //
  return l;
});
auto __ = collide.define<any<thing>, DeathStar>([](auto, auto r) {  //
  return r;
});
auto __ = collide.define<DeathStar, DeathStar>([](auto l, auto r) {  //
  return Asteroid{l.n_ + "&" + r.n_};
});

struct BlackHole {
  std::string n_;
  auto registry() const { return "Black Hole " + n_; }
};
auto __ = collide.define<BlackHole, any<thing>>([](auto l, auto) {  //
  return l;
});
auto __ = collide.define<any<thing>, BlackHole>([](auto, auto r) {  //
  return r;
});
auto __ = collide.define<BlackHole, BlackHole>([](auto l, auto r) {  //
  return BlackHole{l.n_ + " | " + r.n_};
});
}  // namespace

TEST_CASE("talk_2310_bonus_multi_dispatch") {
  auto winner = run({Asteroid{"Chicxulub"}, Spaceship{"NCC-1701"},
                     Spaceship{"USGS Powell"}, DeathStar{"2"},
                     Asteroid{"Tunguska"}, DeathStar{"1"},
                     BlackHole{"Sagittarius A*"}, BlackHole{"Cygnus X-1"}});
  CHECK(winner.registry() == "Black Hole Sagittarius A* | Cygnus X-1");
}
