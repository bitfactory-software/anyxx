#include <catch.hpp>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/factory.hpp>
#include <virtual_void/utillities/unnamed__.hpp>
#

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void;
using namespace virtual_void;
using namespace virtual_void;

namespace {}

VV_V_TABLE_HAS_EXTENSION_METHODS(, Thing)

namespace {

VV_INTERFACE(Thing, )

class Asteroid {};

class Spaceship {};

factory<Thing, std::type_index, std::string> thing_factory;

};  // namespace

namespace {

TEST_CASE("virtual_typed/interface/factory") {}

}  // namespace
