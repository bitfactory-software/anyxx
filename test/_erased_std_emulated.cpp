#include <catch.hpp>
#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/call_operator.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;

namespace virtual_void {
VV_INTERFACE(string_to_string, (VV_METHOD_(std::string, op_call_1, operator(),
                                           const, std::string const&)))

VV_INTERFACE(string_to_string_mutable,
             (VV_METHOD_(std::string, op_call_1, operator(), const),
              VV_METHOD_(std::string, op_call_2, operator(), ,
                         std::string const&)))

template <typename SIG, is_constness CONSTNESS = const_>
using function = interface::call_operator<data::shared_const, SIG, CONSTNESS>;
template <typename SIG, is_constness CONSTNESS = mutable_>
using ref_function =
    interface::call_operator<data::mutable_observer, SIG, CONSTNESS>;
template <typename SIG, is_constness CONSTNESS = mutable_>
using move_only_function =
    interface::call_operator<data::unique, SIG, CONSTNESS>;
}  // namespace virtual_void

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
VV_RUNTIME_STATIC(functor_t)
VV_V_TABLE_INSTANCE(, functor_t, string_to_string_mutable)

VV_RUNTIME_STATIC(pure_functor_t)
VV_V_TABLE_INSTANCE(, pure_functor_t, string_to_string)

VV_RUNTIME_STATIC(pure_functor_with_context)
VV_V_TABLE_INSTANCE(, pure_functor_with_context, string_to_string)

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
    REQUIRE(interface::unerase_cast<functor_t>(f)->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo world");
    REQUIRE(f() == "hallo world");
    REQUIRE(interface::unerase_cast<functor_t>(f)->s_ == "hallo world");

    string_to_string_mutable<data::const_observer> fc{f};
    REQUIRE(fc() == "hallo world");
  }
  {
    pure_functor_t ff{};
    string_to_string<data::mutable_observer> f{ff};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    string_to_string_mutable<unique> f{
        std::make_unique<functor_t>("hello")};
    REQUIRE(f(" world") == "hello");
    REQUIRE(interface::unerase_cast<functor_t>(f)->s_ == "hello world");
    static_assert(!std::assignable_from<
                  string_to_string_mutable<unique>,
                  string_to_string_mutable<unique>>);
    string_to_string_mutable<unique> f2{std::move(f)};
    REQUIRE(!has_data(get_erased_data(f)));
    REQUIRE(f2(", bye") == "hello world");
    REQUIRE(interface::unerase_cast<functor_t>(f2)->s_ == "hello world, bye");
  }
}
