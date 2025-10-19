#include <anyxx/anyxx.hpp>
#include <catch.hpp>

using namespace Catch::Matchers;
using namespace anyxx;
using namespace std::literals;

namespace {}

ANY_HAS_DISPATCH(, any_thing)
namespace {
ANY(any_thing, )
}  // namespace

namespace {}  // namespace

namespace {

namespace {
class Asteroid {};
}  // namespace
namespace {
class Spaceship {};
}  // namespace

// auto __ = collide.define<any_thing<const_observer>,
// any_thing<const_observer>>(
//     [](auto a, auto s) { return "(*, *) default"; });
// auto __ =
//    collide.define<Asteroid, Spaceship>([](auto a, auto s) { return "a->s";
//    });
// auto __ =
//    collide.define<Asteroid, Asteroid>([](auto a, auto s) { return "a->a"; });
// auto __ =
//    collide.define<Spaceship, Spaceship>([](auto a, auto s) { return "s->s";
//    });
// auto __ =
//    collide.define<Spaceship, Asteroid>([](auto a, auto s) { return "s->a";
//    });

// great time to use the spaceship operator...
// std::string operator<=>(any_thing<const_observer> const& l,
//                        any_thing<const_observer> const& r) {
//  return collide(l, r);
//}

TEST_CASE("multi_dispatch 1") {
  dispatch<std::string(virtual_<any_thing<const_observer>>,
                       virtual_<any_thing<const_observer>>)>
      collide;

  auto __ = collide.define<Asteroid, Spaceship>(
      [](auto a, auto s) { return "a->s"; });
  auto __ =
      collide.define<Asteroid, Asteroid>([](auto a, auto s) { return "a->a"; });
  auto __ = collide.define<Spaceship, Spaceship>(
      [](auto a, auto s) { return "s->s"; });
  auto __ = collide.define<Spaceship, Asteroid>(
      [](auto a, auto s) { return "s->a"; });

  CHECK(any_thing_v_table::imlpementation<Asteroid>()
            ->own_dispatch_holder_t::dispatch_table->size() == 3);
  CHECK(any_thing_v_table::imlpementation<Spaceship>()
            ->own_dispatch_holder_t::dispatch_table->size() == 3);

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<const_observer> thing_asteroid{asteroid},
      thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "a->s");  // a->s");
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship) == "s->s");
  CHECK(collide(thing_spaceship, thing_asteroid) == "s->a");

  // CHECK((thing_asteroid <=> thing_spaceship) == "(*, *) default");  //
  // a->s"); CHECK((thing_asteroid <=> thing_asteroid) == "a->a");
  // CHECK((thing_spaceship <=> thing_spaceship) == "s->s");
  // CHECK((thing_spaceship <=> thing_asteroid) == "s->a");
}

TEST_CASE("multi_dispatch 2") {
  dispatch<std::string(virtual_<any_thing<const_observer>>,
                       virtual_<any_thing<const_observer>>)>
      collide;

  auto __ = collide.define<Asteroid, any_thing<const_observer>>(
      [](auto a, auto s) { return "default"; });
  auto __ =
      collide.define<Asteroid, Asteroid>([](auto a, auto s) { return "a->a"; });
  auto __ = collide.define<Spaceship, Spaceship>(
      [](auto a, auto s) { return "s->s"; });
  auto __ = collide.define<Spaceship, Asteroid>(
      [](auto a, auto s) { return "s->a"; });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<const_observer> thing_asteroid{asteroid},
      thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "default");
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship) == "s->s");
  CHECK(collide(thing_spaceship, thing_asteroid) == "s->a");
}

TEST_CASE("multi_dispatch 3") {
  dispatch<std::string(virtual_<any_thing<const_observer>>,
                       virtual_<any_thing<const_observer>>)>
      collide;

  auto __ = collide.define<Asteroid, any_thing<const_observer>>(
      [](auto a, auto s) { return "default"; });
  auto __ =
      collide.define<Asteroid, Asteroid>([](auto a, auto s) { return "a->a"; });
  auto __ = collide.define<any_thing<const_observer>, Spaceship>(
      [](auto a, auto s) { return "s->s"; });
  auto __ = collide.define<any_thing<const_observer>, Asteroid>(
      [](auto a, auto s) { return "s->a"; });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<const_observer> thing_asteroid{asteroid},
      thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "default");
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship) == "s->s");
  CHECK(collide(thing_spaceship, thing_asteroid) == "s->a");
}

TEST_CASE("multi_dispatch 4") {
  dispatch<std::string(virtual_<any_thing<const_observer>>,
                       virtual_<any_thing<const_observer>>)>
      collide;

  auto __ =
      collide.define<Asteroid, Asteroid>([](auto a, auto s) { return "a->a"; });
  auto __ = collide.define<any_thing<const_observer>, Spaceship>(
      [](auto any, auto s) {
        return get_meta_data(*any).get_type_info().name() + "->s"s;
      });
  auto __ = collide.define<any_thing<const_observer>, Asteroid>(
      [](auto any, auto s) { return "any->a"; });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<const_observer> thing_asteroid{asteroid},
      thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship).contains("Asteroid"));
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship).contains("Spaceship"));
  CHECK(collide(thing_spaceship, thing_asteroid) == "any->a");
}

}  // namespace

// some compile time self tests

namespace {
ANY(Dummy, )

template <typename... ARGS>
struct have_dispatchs_enabled {
  static constexpr bool value = true;
};
template <is_any ANY, typename... ARGS>
  requires has_dispatchs_enabled<ANY>
struct have_dispatchs_enabled<virtual_<ANY>, ARGS...>
    : have_dispatchs_enabled<ARGS...> {};
template <is_any ANY, typename... ARGS>
  requires(!has_dispatchs_enabled<ANY>)
struct have_dispatchs_enabled<virtual_<ANY>, ARGS...> {
  static constexpr bool value = false;
};
static_assert(!has_dispatchs_enabled<Dummy<const_observer>>);
static_assert(has_dispatchs_enabled<any_thing<const_observer>>);

static_assert(!have_dispatchs_enabled<virtual_<Dummy<const_observer>>,
                                      virtual_<Dummy<const_observer>>>::value);
static_assert(
    !have_dispatchs_enabled<virtual_<Dummy<const_observer>>,
                            virtual_<any_thing<const_observer>>>::value);
static_assert(!have_dispatchs_enabled<virtual_<any_thing<const_observer>>,
                                      virtual_<Dummy<const_observer>>>::value);
static_assert(
    have_dispatchs_enabled<virtual_<any_thing<const_observer>>,
                           virtual_<any_thing<const_observer>>>::value);

}  // namespace
