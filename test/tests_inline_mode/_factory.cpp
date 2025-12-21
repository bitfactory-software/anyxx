#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <print>
#include <vector>

using namespace anyxx;
using namespace std::literals;

namespace {
namespace example {

ANY(any_thing, , , )

class asteroid {};

class spaceship {};

ANY_SINGELTON_DECLARE(, thing_factory, factory<any_thing, std::string>);

auto __ = thing_factory.register_("asteroid", []() {
  return any_thing<unique>{std::make_unique<asteroid>()};
});
auto __ = thing_factory.register_("spaceship", []() {
  return any_thing<unique>{std::make_unique<spaceship>()};
});
}}  // namespace

namespace {
namespace example {

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

ANY(any_to_string,
    (ANY_METHOD_DEFAULTED(std::string, to_string, (), const,
                          [x]() { return std::format("{}", x); })), , )

ANY_SINGELTON_DECLARE(, any_to_string_factory,
                      factory<any_to_string, std::string>);

auto __ = any_to_string_factory.register_(
    "int", []() { return any_to_string<unique>{std::in_place, 42}; });

static_assert(std::is_constructible_v<any_to_string<shared_const>,
                                      any_to_string<unique>&&>);
static_assert(!std::is_constructible_v<any_to_string<shared_const, dynm>,
                                       any_to_string<unique>&&>);
}}  // namespace

ANY_SINGELTON(example, thing_factory);
ANY_SINGELTON(example, any_to_string_factory);
