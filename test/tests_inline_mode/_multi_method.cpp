#include <catch.hpp>
#include <anypp/any++.hpp>

using namespace Catch::Matchers;

using namespace anypp;

namespace {}

ANY_V_TABLE_HAS_EXTENSION_METHODS(, Thing)
namespace {
ANY(Thing, )
}  // namespace

namespace {
extension_method<
             std::string(virtual_<Thing<const_observer>>, virtual_<Thing<const_observer>>)>
    collide;
}  // namespace

namespace {

namespace {
class Asteroid {};
}  // namespace
namespace {
class Spaceship {};
}  // namespace

//auto __ =
//    collide.define<Asteroid, Spaceship>([](auto a, auto s) { return "a->s"; });
auto __ =
    collide.define<Asteroid, Asteroid>([](auto a, auto s) { return "a->a"; });
auto __ =
    collide.define<Spaceship, Spaceship>([](auto a, auto s) { return "s->s"; });
auto __ =
    collide.define<Spaceship, Asteroid>([](auto a, auto s) { return "s->a"; });

TEST_CASE("multi_method") {
  CHECK(Thing_v_table::imlpementation<Asteroid>()
            ->own_extension_method_holder_t::extension_method_table->size() ==
        2);
  CHECK(Thing_v_table::imlpementation<Spaceship>()
            ->own_extension_method_holder_t::extension_method_table->size() ==
        2);

  Asteroid asteroid;
  Spaceship spaceship;

  Thing<const_observer> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == ""); //a->s");
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship) == "s->s");
  CHECK(collide(thing_spaceship, thing_asteroid) == "s->a");
}

}  // namespace

// some compile time self tests

namespace {
ANY(Dummy, )

template <typename... ARGS>
struct have_extension_methods_enabled {
  static constexpr bool value = true;
};
template <is_any ANY, typename... ARGS>
  requires has_extension_methods_enabled<ANY>
struct have_extension_methods_enabled<virtual_<ANY>, ARGS...>
    : have_extension_methods_enabled<ARGS...> {};
template <is_any ANY, typename... ARGS>
  requires(!has_extension_methods_enabled<ANY>)
struct have_extension_methods_enabled<virtual_<ANY>, ARGS...> {
  static constexpr bool value = false;
};
static_assert(!has_extension_methods_enabled<Dummy<const_observer>>);
static_assert(has_extension_methods_enabled<Thing<const_observer>>);

static_assert(
    !have_extension_methods_enabled<virtual_<Dummy<const_observer>>,
                                    virtual_<Dummy<const_observer>>>::value);
static_assert(
    !have_extension_methods_enabled<virtual_<Dummy<const_observer>>,
                                    virtual_<Thing<const_observer>>>::value);
static_assert(
    !have_extension_methods_enabled<virtual_<Thing<const_observer>>,
                                    virtual_<Dummy<const_observer>>>::value);
static_assert(
    have_extension_methods_enabled<virtual_<Thing<const_observer>>,
                                   virtual_<Thing<const_observer>>>::value);

}  // namespace
