#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ANY_SINGLETON.hpp>
#include <bit_factory/anyxx/ext/factory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <print>
#include <vector>

using namespace anyxx;
using namespace std::literals;

namespace {
namespace example {

TRAIT(thing, dynamic_deletable, )

class asteroid {};

class spaceship {};

ANY_SINGLETON_DECLARE(, thing_factory,
                      factory<any_<unique, thing>, std::string>);

auto __ = thing_factory.register_("asteroid", []() { return asteroid{}; });
auto __ = thing_factory.register_("spaceship", []() { return spaceship{}; });
}  // namespace example
}  // namespace

namespace {
namespace example {

TEST_CASE("factory1") {
  auto asteroid_thing = thing_factory.construct("asteroid");
  CHECK(any_cast_if<asteroid>(asteroid_thing) != nullptr);
  CHECK(type_of(asteroid_thing) == typeid(asteroid));
  auto spaceship_thing = thing_factory.construct("spaceship");
  CHECK(any_cast_if<spaceship>(spaceship_thing) != nullptr);
  CHECK(type_of(spaceship_thing) == typeid(spaceship));
}

TEST_CASE("factory2") {
  std::vector<any_<unique, thing>> things;
  std::stringstream archive{"asteroid spaceship"};
  while (archive) {
    std::string key;
    archive >> key;
    if (!key.empty()) things.emplace_back(thing_factory.construct(key));
  }
  CHECK(type_of(things[0]) == typeid(asteroid));
  CHECK(type_of(things[1]) == typeid(spaceship));
}

TRAIT(stringable, dynamic_deletable,
      (FN_IMPL(
          public, std::string, to_string, (), const,
          () { return std::format("{}", self); })))

ANY_SINGLETON_DECLARE(, any_stringable_factory,
                      factory<any_<unique, stringable>, std::string>);

auto __ = any_stringable_factory.register_("int", []() { return 42; });

static_assert(std::is_constructible_v<any_<shared, stringable>,
                                      any_<unique, stringable>&&>);
}  // namespace example
}  // namespace

ANY_SINGLETON(example, thing_factory);
ANY_SINGLETON(example, any_stringable_factory);

namespace {
namespace example {

struct tag_factory_test;
using factory_test_key_kind = key<tag_factory_test>;

ANY_SINGLETON_DECLARE(, factory_test_key, factory_test_key_kind)
ANY_SINGLETON_DECLARE(, factory_test_negative_key, factory_test_key_kind)

TEST_CASE("factory3") {
  factory<any_<unique, stringable>, factory_test_key_kind> f;
  f.register_(factory_test_key, []() { return 42; });
  auto a1 = f.construct(factory_test_key);
  CHECK(a1.to_string() == "42");
  CHECK_THROWS_AS(f.construct(factory_test_negative_key),
                  unknown_factory_key_error);
}

}  // namespace example
}  // namespace

ANY_SINGLETON(example, factory_test_key, "factory_int_key")
ANY_SINGLETON(example, factory_test_negative_key, "factory_test_negative_key")
