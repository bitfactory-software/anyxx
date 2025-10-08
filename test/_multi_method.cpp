#include <catch.hpp>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/multi_method.hpp>
#

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::runtime;
using namespace virtual_void::data;
using namespace virtual_void::interface;

VV_V_TABLE_INSTANCE_ON_THE_FLY(, Thing)
VV_V_TABLE_HAS_EXTENSION_METHODS(, Thing)

namespace {

VV_INTERFACE(Thing, )

class Asteroid {};

class Spaceship {};

};  // namespace

VV_EXTENSION_METHOD_COUNT(Thing_v_table)

VV_EXTENSION_TABLE_INSTANCE(Asteroid, Thing_v_table)
VV_EXTENSION_TABLE_INSTANCE(Spaceship, Thing_v_table)

namespace {

TEST_CASE("virtual_typed/interface/multi_method") {}
}  // namespace
