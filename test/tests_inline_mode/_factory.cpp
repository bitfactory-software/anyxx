#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <print>
#include <vector>

using namespace anyxx;
using namespace std::literals;

namespace {
namespace example {

ANY(thing, , )

class asteroid {};

class spaceship {};

ANY_SINGLETON_DECLARE(, thing_factory, factory<any_thing, std::string>);

auto __ = thing_factory.register_("asteroid", []() { return asteroid{}; });
auto __ = thing_factory.register_("spaceship", []() { return spaceship{}; });
}  // namespace example
}  // namespace

namespace {
namespace example {

TEST_CASE("factory1") {
  auto asteroid_thing = thing_factory.construct<unique>("asteroid");
  CHECK(unerase_cast<asteroid>(asteroid_thing) != nullptr);
  CHECK(get_type_info(asteroid_thing) == typeid(asteroid));
  auto spaceship_thing = thing_factory.construct<unique>("spaceship");
  CHECK(unerase_cast<spaceship>(spaceship_thing) != nullptr);
  CHECK(get_type_info(spaceship_thing) == typeid(spaceship));
}

TEST_CASE("factory2") {
  std::vector<any_thing<unique>> things;
  std::stringstream archive{"asteroid spaceship"};
  while (archive) {
    std::string key;
    archive >> key;
    if (!key.empty()) things.emplace_back(thing_factory.construct<unique>(key));
  }
  CHECK(get_type_info(things[0]) == typeid(asteroid));
  CHECK(get_type_info(things[1]) == typeid(spaceship));
}

ANY(stringable,
    (ANY_METHOD_DEFAULTED(std::string, to_string, (), const,
                          [&x]() { return std::format("{}", x); })),
    )

ANY_SINGLETON_DECLARE(, any_stringable_factory,
                      factory<any_stringable, std::string>);

auto __ = any_stringable_factory.register_("int", []() { return 42; });

static_assert(std::is_constructible_v<any_stringable<shared_const>,
                                      any_stringable<unique>&&>);
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
  factory<any_stringable, factory_test_key_kind> f;
  f.register_(factory_test_key, []() { return 42; });
  auto a1 = f.construct<unique>(factory_test_key);
  CHECK(a1.to_string() == "42");
  CHECK_THROWS_AS(f.construct<unique>(factory_test_negative_key),
                  unkonwn_factory_key_error);
}

}  // namespace example
}  // namespace

ANY_SINGLETON(example, factory_test_key, "factory_int_key")
ANY_SINGLETON(example, factory_test_negative_key, "factory_test_negative_key")
