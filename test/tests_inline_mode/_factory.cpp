#include <anyxx/anyxx.hpp>
#include <catch.hpp>

using namespace Catch::Matchers;
using namespace anyxx;

namespace {

ANY(any_thing, )

class asteroid {};

class spaceship {};

factory<any_thing, std::string> thing_factory;

auto __ = thing_factory.register_("asteroid", []() {
  return any_thing<unique>{std::make_unique<asteroid>()};
});
auto __ = thing_factory.register_("spaceship", []() {
  return any_thing<unique>{std::make_unique<spaceship>()};
});
};  // namespace

namespace {

TEST_CASE("factory") {
  auto asteroid_thing = thing_factory.construct("asteroid");
  CHECK(unerase_cast<asteroid>(asteroid_thing) != nullptr);
  CHECK(get_meta_data(asteroid_thing).get_type_info() == typeid_of<asteroid>());
}

}  // namespace
