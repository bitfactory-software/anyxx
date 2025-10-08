#include <catch.hpp>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/multi_method.hpp>
#include <virtual_void/utillities/unnamed__.hpp>
#

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::runtime;
using namespace virtual_void::data;
using namespace virtual_void::interface;

namespace {}

VV_V_TABLE_INSTANCE_ON_THE_FLY(, Thing)
VV_V_TABLE_HAS_EXTENSION_METHODS(, Thing)

namespace {

VV_INTERFACE(Thing, )

class Asteroid {};

class Spaceship {};

};  // namespace

VV_EXTENSION_METHOD_COUNT(Thing)

VV_EXTENSION_TABLE_INSTANCE(Asteroid, Thing)
VV_EXTENSION_TABLE_INSTANCE(Spaceship, Thing)

multi_method<Thing<const_observer>, Thing<const_observer>,
             std::string(virtual_void::const_, virtual_void::const_)>
    collide;

namespace {

auto __ =
    collide.define<Asteroid, Spaceship>([](auto a, auto s) { return "a->s"; });
auto __ =
    collide.define<Asteroid, Asteroid>([](auto a, auto s) { return "a->a"; });
auto __ =
    collide.define<Spaceship, Spaceship>([](auto a, auto s) { return "a->a"; });
auto __ =
    collide.define<Spaceship, Asteroid>([](auto a, auto s) { return "a->a"; });

TEST_CASE("virtual_typed/interface/multi_method") {
  CHECK(Thing_v_table::imlpementation<Asteroid>()
            ->own_extension_method_holder_t::extension_method_table->size() ==
        2);
  CHECK(Thing_v_table::imlpementation<Spaceship>()
            ->own_extension_method_holder_t::extension_method_table->size() ==
        2);

  Asteroid asteroid;
  Spaceship spaceship;

  Thing<const_observer> thing_asteroid{asteroid}, thing_spaceship{spaceship};

  CHECK(collide(thing_asteroid, thing_spaceship) == "a->s");
  CHECK(collide(thing_asteroid, thing_asteroid) == "a->a");
  CHECK(collide(thing_spaceship, thing_spaceship) == "s->s");
  CHECK(collide(thing_spaceship, thing_asteroid) == "s->a");
}

}  // namespace
