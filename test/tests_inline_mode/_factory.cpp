#include <catch.hpp>
#include <anypp/any++.hpp>
#

using namespace Catch::Matchers;

using namespace anypp;
using namespace anypp;
using namespace anypp;
using namespace anypp;

namespace {}

ANY_V_TABLE_HAS_EXTENSION_METHODS(, Thing)

namespace {

ANY(Thing, )

class Asteroid {};

class Spaceship {};

factory<Thing, std::type_index, std::string> thing_factory;

};  // namespace

namespace {

TEST_CASE("factory") {}

}  // namespace
