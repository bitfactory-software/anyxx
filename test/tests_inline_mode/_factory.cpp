#include <anyxx/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <print>
#include <ranges>
#include <vector>

using namespace anyxx;
using namespace std::literals;

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

TEST_CASE("factory1") {
  auto asteroid_thing = thing_factory.construct("asteroid");
  CHECK(unerase_cast<asteroid>(asteroid_thing) != nullptr);
  CHECK(get_meta_data(asteroid_thing).get_type_info() == typeid_of<asteroid>());
  auto spaceship_thing = thing_factory.construct("spaceship");
  CHECK(unerase_cast<spaceship>(spaceship_thing) != nullptr);
  CHECK(get_meta_data(spaceship_thing).get_type_info() ==
        typeid_of<spaceship>());
}

TEST_CASE("factory2") {
  std::vector<any_thing<unique>> things;
  std::stringstream archive{"asteroid spaceship"};
  while (archive) {
    std::string key;
    archive >> key;
    if (!key.empty()) things.emplace_back(thing_factory.construct(key));
  }
  CHECK(get_meta_data(things[0]).get_type_info() == typeid_of<asteroid>());
  CHECK(get_meta_data(things[1]).get_type_info() == typeid_of<spaceship>());
}

// TEST_CASE("factory3") {

}  // namespace
