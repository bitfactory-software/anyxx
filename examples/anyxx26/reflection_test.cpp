#include <meta>
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <print>

#include <catch2/catch_test_macros.hpp>

#include <bit_factory/anyxx.hpp>
#include <examples/anyxx26/meta/utilities.hpp>
#include <examples/anyxx26/trait_as.hpp>
#include <examples/anyxx26/dyn.hpp>

using namespace anyxx26;

template <typename Self, bool base = true>
struct stringable{
    [[=defaulted]] static std::string as_string(Self const& self);
};

void print(std::vector<dyn<stringable>> const& things){
    for(auto& thing : things){
        std::println("{}", thing.as_string());
    }
}

template <>
struct stringable<int>{
    static std::string as_string(int const& self) {
        return std::to_string(self);
    }
};

template <>
struct stringable<std::string>{
    static std::string as_string(std::string const& self) {
        return self;
    }
};

struct foo{ double f; };
template <>
struct stringable<foo>{
    static std::string as_string(foo const& self){
        return "foo: " + std::to_string(self.f);
    }
};

struct boo {
    bool b = false;
    std::string as_string(){
        return std::string{ "boo? " } + (b ? "T" : "F");
    }
};
// Only to show how to delegate to the default adapter:
template <>
struct stringable<boo, false> : stringable<boo, true>{};

TEST_CASE("refelction hello world") {

    auto a1 = trait_as<int, stringable>{ 42 };
    auto z_from_self = a1.as_string();
    std::println("z_from_trait = {}", z_from_self);

    static_assert(std::is_same_v<base_v_table_t<stringable>, anyxx::observeable_v_table>);

    int i = 4711;
    auto dyn_stringable = dyn<stringable>{ i };
    auto z_from_dyn_stringable = dyn_stringable.as_string();
    std::println("z_from_dyn_stringable = {}", z_from_dyn_stringable);

    std::string s = "hello world";
    foo a_foo{ 3.14 };
    boo a_boo{ true };
    print({ dyn<stringable>{i}, dyn<stringable>{s}, dyn<stringable>{a_foo}, dyn<stringable>{a_boo} });
}
