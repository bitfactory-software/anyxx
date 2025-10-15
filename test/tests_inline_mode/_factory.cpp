#include <catch.hpp>
#include <scn/scan.h>
#include <print>
#include <anyxx/anyxx.hpp>


using namespace Catch::Matchers;
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
}

TEST_CASE("factory2") {

// Reading a std::string will read until the first whitespace character
    if (auto result = scn::scan<std::string>("Hello world!", "{}")) {
        // Will output "Hello":
        // Access the read value with result->value()
        std::println("{}", result->value());
        
        // Will output " world":
        // result->range() returns a subrange containing the unused input
        // C++23 is required for the std::string_view range constructor used below
        std::println("{}", std::string_view{result->range()});
    } else {
        std::println("Couldn't parse a word: {}", result.error().msg());
    }

    //std::vector<int> vec{};
    //auto input = scn::ranges::subrange{"123 456 789"sv};
    //
    //while (auto result = scn::scan<int>(input), "{}")) {
    //    vec.push_back(result->value());
    //    input = result->range();
    //}
//while (auto result = scn::scan<std::string>("asteroid spaceship"), "{}")) {
  //auto asteroid_thing = thing_factory.construct("asteroid");
  //CHECK(unerase_cast<asteroid>(asteroid_thing) != nullptr);
  //CHECK(get_meta_data(asteroid_thing).get_type_info() == typeid_of<asteroid>());
}


}  // namespace
