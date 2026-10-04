#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/function.hpp>
#include <bit_factory/v26/meta/print_members.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <bit_factory/v26/trait_as.hpp>
#include <catch2/catch_test_macros.hpp>
#include <meta>
#include <print>
#include <string>
#include <utility>
#include <vector>

using namespace anyxx26;

namespace {

// base architecture layer
template <is_trait Trait, typename Self>
struct base : dynamic_castable<Trait, Self> {
    std::string as_string() const;
};

std::string process_with(std::vector<any<base>> const& things,
                         any<function, std::string(any<base> const&)> process) {
    std::string result;
    for(auto const& thing : things) {
        result += process(thing) + "\n";
    }
    return result;
}

// basic implementation layer
template <>
struct base<model_map, int> {
    static std::string as_string(int const& self) { return std::to_string(self); }
};

template <>
struct base<model_map, std::string> {
    static std::string as_string(std::string const& self) { return self; }
};

// higher architecture layer
template <is_trait Trait, typename Self>
struct derived : base<Trait, Self> {
    std::string decorated() const;
};

std::string decoration_handler(any<base> const& thing) {
    if(auto d = anyxx::downcast_to<any<derived>>(thing)) {
        return d->decorated();
    } else {
        return "possible answer: " + thing.as_string();
    }
};

// higher implementation layer
template <>
struct derived<model_map, int> {
    static std::string decorated(int const& self) {
        return "int: " + std::to_string(self);
    }
};

template <>
struct derived<model_map, std::string> {
    static std::string decorated(std::string const& self) {
        return "string: " + self;
    }
};

}  // namespace

TEST_CASE("anyxx26 downcast") {

    int i = 4711;
    std::string s = "hello world";

    auto processed =
        process_with({ any<derived>{i}, any<derived>{s}, 42 }, decoration_handler);
    CHECK(processed == "int: 4711\nstring: hello world\npossible answer: 42\n");
}
