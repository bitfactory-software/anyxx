#include <catch.hpp>
#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;

namespace {
VV_INTERFACE(string_to_string,
             (VV_CONST_OP(std::string, 1, (), std::string const&)))

VV_INTERFACE(string_to_string_mutable,
             (VV_CONST_OP(std::string, 1, ()),
              VV_OP(std::string, 2, (), std::string const&)))

}  // namespace test_ns

using namespace virtual_void;

namespace {
struct functor_t {
  std::string s_;
  std::string operator()() const { return s_; }
  std::string operator()(const std::string& s) {
    auto ret = s_;
    s_ += s;
    return ret;
  }
};
struct pure_functor_t {
  std::string operator()() const { return "literal"; }
  std::string operator()(const std::string& s) const { return s; }
};
struct pure_functor_with_context {
  std::string s_;
  std::string operator()(const std::string& s) const { return s_ + s; }
};
}  // namespace

TEST_CASE("std emulated function") {
  {
    auto functor = std::make_shared<pure_functor_with_context>("hallo");
    string_to_string<data::shared_const> f{functor};
    REQUIRE(f(" world") == "hallo world");
  }
  {
    string_to_string<data::shared_const> f{std::make_shared<pure_functor_t>()};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    functor_t functor{"hallo"};
    string_to_string_mutable<data::mutable_observer> f{functor};
    REQUIRE(interface::unchecked_unerase_cast<functor_t>(f)->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo world");
    REQUIRE(f() == "hallo world");
    REQUIRE(interface::unchecked_unerase_cast<functor_t>(f)->s_ ==
            "hallo world");

    string_to_string_mutable<data::const_observer> fc{f};
    REQUIRE(fc() == "hallo world");

    functor_t fx{"hallo world"};
    string_to_string_mutable<data::const_observer> fc1{fx};
    REQUIRE(fc1() == "hallo world");

    // does not work, because functor lives only during construction of
    // string_to_string_mutable
    // string_to_string_mutable<data::const_observer> fc2{functor_t{"hallo
    // world"}}; CHECK(fc2() == "hallo world"); // access of member is invalid
  }
  {
    pure_functor_t pf{};
    string_to_string<data::mutable_observer> f{pf};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    string_to_string<data::const_observer> f{
        pure_functor_t{}};  // thos works, because 'pure'
    REQUIRE(f("hello world") == "hello world");
  }
  {
    string_to_string_mutable<unique> f{std::make_unique<functor_t>("hello")};
    REQUIRE(f(" world") == "hello");
    REQUIRE(interface::unchecked_unerase_cast<functor_t>(f)->s_ ==
            "hello world");
    static_assert(!std::assignable_from<string_to_string_mutable<unique>,
                                        string_to_string_mutable<unique>>);
    string_to_string_mutable<unique> f2{std::move(f)};
    REQUIRE(!has_data(get_erased_data(f)));
    REQUIRE(f2(", bye") == "hello world");
    REQUIRE(interface::unchecked_unerase_cast<functor_t>(f2)->s_ ==
            "hello world, bye");
  }

  {
    string_to_string<const_observer> sts{[](std::string const& in) {
      return in + " world!";
    }};  // works, because pure
    CHECK(sts("hello") == "hello world!");
  }
}
