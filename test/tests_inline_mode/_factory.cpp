#include <catch.hpp>
#include <anyxx/anyxx.hpp>
#

using namespace Catch::Matchers;

using namespace anyxx;
using namespace anyxx;
using namespace anyxx;
using namespace anyxx;

namespace {}

ANY_HAS_METHODS(, Thing)

namespace {

ANY(Thing, )

class Asteroid {};

class Spaceship {};

factory<Thing, std::type_index, std::string> thing_factory;

};  // namespace

namespace {

TEST_CASE("factory") {}

}  // namespace
