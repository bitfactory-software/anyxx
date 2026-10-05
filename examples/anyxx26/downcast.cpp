#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/function.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>
#include <vector>

using namespace anyxx26;

namespace {

// base architecture layer
template <is_trait Trait, typename Self>
struct base : dynamic_castable<Trait, Self> {
    static std::string as_string(Self const& self){
        return std::format("{}", self);
    }
};

std::string process_with(std::vector<any<base>> const& things,
                         any<function, std::string(any<base> const&)> process) {
    std::string result;
    for(auto const& thing : things) {
        result += process(thing) + "\n";
    }
    return result;
}

// higher architecture layer
template <is_trait Trait, typename Self>
struct derived : base<Trait, Self> {
    static std::string decorated(Self const& self) {
        std::string decoration = std::define_static_string(std::meta::display_string_of(^^Self));
        return decoration + ": " + any<base>{self}.as_string();
    }
};

std::string decoration_handler(any<base> const& thing) {
    if(auto d = anyxx::downcast_to<any<derived>>(thing)) {
        return d->decorated();
    } else {
        return "possible answer: " + thing.as_string();
    }
};

}  // namespace

TEST_CASE("anyxx26 downcast") {

    int i = 4711;
    std::string s = "hello world";

    auto processed =
        process_with({ any<derived>{i}, any<derived>{s}, 42 }, decoration_handler);
    CHECK(processed == "int: 4711\nstd::__cxx11::basic_string<char>: hello world\npossible answer: 42\n");
}
