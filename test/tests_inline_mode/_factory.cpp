#include <catch.hpp>
#include <virtual_void/observer.hpp>
#include <virtual_void/shared_const.hpp>
#include <virtual_void/any_meta_function.hpp>
#include <virtual_void/factory.hpp>
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

VV_ANY(Thing, )

class Asteroid {};

class Spaceship {};

factory<Thing, std::type_index, std::string> thing_factory;

};  // namespace

namespace {

TEST_CASE("factory") {}

}  // namespace
