#include <catch.hpp>
#include <anypp/observer.hpp>
#include <anypp/shared_const.hpp>
#include <anypp/any_meta_class.hpp>
#include <anypp/factory.hpp>
#include <anypp/utillities/unnamed__.hpp>
#

using namespace Catch::Matchers;

using namespace anypp;
using namespace anypp;
using namespace anypp;
using namespace anypp;

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
