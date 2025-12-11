#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#ifdef _MSC_VER
#pragma warning( \
    disable : 4189)  // local variable is initialized but not referenced
#endif

using namespace anyxx;
using namespace std::literals;

namespace {
struct any_thing_has_dispatch {};
ANY(any_thing, (ANY_METHOD(std::string, name, (), const)))
}  // namespace

namespace {}  // namespace

namespace {

namespace {
struct Asteroid {
  static std::string name() { return "Asteroid"; }
};
}  // namespace
namespace {
struct Spaceship {
  static std::string name() { return "Spaceship"; }
};

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

  collide.define<Asteroid, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Asteroid, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Spaceship, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Spaceship, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });

  CHECK(any_thing_v_table<>::imlpementation<Asteroid>()
            ->own_dispatch_holder_t::dispatch_table->size() == 3);
  CHECK(any_thing_v_table<>::imlpementation<Spaceship>()
            ->own_dispatch_holder_t::dispatch_table->size() == 3);

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<const_observer> thing_asteroid{asteroid},
      thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "Asteroid->Spaceship");
  CHECK(collide(thing_asteroid, thing_asteroid) == "Asteroid->Asteroid");
  CHECK(collide(thing_spaceship, thing_spaceship) == "Spaceship->Spaceship");
  CHECK(collide(thing_spaceship, thing_asteroid) == "Spaceship->Asteroid");

  // CHECK((thing_asteroid <=> thing_spaceship) == "(*, *) default");  //
  // a->s"); CHECK((thing_asteroid <=> thing_asteroid) == "a->a");
  // CHECK((thing_spaceship <=> thing_spaceship) == "s->s");
  // CHECK((thing_spaceship <=> thing_asteroid) == "s->a");
}

TEST_CASE("multi_dispatch 2") {
  dispatch<std::string(virtual_<any_thing<const_observer>>,
                       virtual_<any_thing<const_observer>>)>
      collide;

  collide.define<Asteroid, any_thing<const_observer>>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + ":A->*" + r.name();
      });
  collide.define<Asteroid, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Spaceship, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<Spaceship, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<const_observer> thing_asteroid{asteroid},
      thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "Asteroid:A->*Spaceship");
  CHECK(collide(thing_asteroid, thing_asteroid) == "Asteroid->Asteroid");
  CHECK(collide(thing_spaceship, thing_spaceship) == "Spaceship->Spaceship");
  CHECK(collide(thing_spaceship, thing_asteroid) == "Spaceship->Asteroid");
}

TEST_CASE("multi_dispatch 3") {
  dispatch<std::string(virtual_<any_thing<const_observer>>,
                       virtual_<any_thing<const_observer>>)>
      collide;

  collide.define<Asteroid, any_thing<const_observer>>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + ":A->*" + r.name();
      });
  collide.define<Asteroid, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "->" + r.name();
      });
  collide.define<any_thing<const_observer>, Spaceship>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "*->S:" + r.name();
      });
  collide.define<any_thing<const_observer>, Asteroid>(
      []([[maybe_unused]] auto l, [[maybe_unused]] auto r) {
        return l.name() + "*->A:" + r.name();
      });

  Asteroid asteroid;
  Spaceship spaceship;

  any_thing<const_observer> thing_asteroid{asteroid},
      thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "Asteroid:A->*Spaceship");
  CHECK(collide(thing_asteroid, thing_asteroid) == "Asteroid->Asteroid");
  CHECK(collide(thing_spaceship, thing_spaceship) == "Spaceship*->S:Spaceship");
  CHECK(collide(thing_spaceship, thing_asteroid) == "Spaceship*->A:Asteroid");
}

TEST_CASE("multi_dispatch 4") {
  dispatch<std::string(virtual_<any_thing<const_observer>>,
                       virtual_<any_thing<const_observer>>)>
      collide;

  collide.define<Asteroid, Asteroid>(
      []([[maybe_unused]] auto a, [[maybe_unused]] auto s) { return "a->a"; });
  collide.define<any_thing<const_observer>, Spaceship>(
      [](auto const& any, [[maybe_unused]] auto const& s) {
        return get_meta_data(any).get_type_info().name() + "->s"s;
      });
  collide.define<any_thing<const_observer>, Asteroid>(
      []([[maybe_unused]] auto const& any, [[maybe_unused]] auto const& a) {
        return "any->a";
      });

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
