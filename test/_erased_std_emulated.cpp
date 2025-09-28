#include <catch.hpp>
#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/data/value.hpp>
#include <virtual_void/interface/call_operator.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;

namespace virtual_void {
template <typename SIG, is_constness CONSTNESS = mutable_>
using function =
    interface::call_operator<data::value, SIG, CONSTNESS>;
template <typename SIG, is_constness CONSTNESS = mutable_>
using ref_function =
    interface::call_operator<data::mutable_observer, SIG,
                             CONSTNESS>;
template <typename SIG, is_constness CONSTNESS = mutable_>
using move_only_function =
    interface::call_operator<data::unique, SIG, CONSTNESS>;
}  // namespace virtual_void

using namespace virtual_void;

namespace {
struct functor_t {
  std::string s_;
  std::string operator()(const std::string& s) {
    auto ret = s_;
    s_ += s;
    return ret;
  }
};
struct pure_functor_t {
  std::string operator()(const std::string& s) {
    return s;
  }
};
}
VV_RUNTIME_STATIC(functor_t)
VV_RUNTIME_STATIC(pure_functor_t)

TEST_CASE("std emulated function") {
  {
    functor_t functor{"hallo"};
    function<std::string(const std::string)> f{functor};
    REQUIRE(interface::unerase_cast<functor_t>(f)->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo");
    REQUIRE(interface::unerase_cast<functor_t>(f)->s_ == "hallo world");
  }
  {
    function<std::string(const std::string)> f{pure_functor_t{}};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    functor_t functor{"hallo"};
    ref_function<std::string(const std::string)> f{functor};
    REQUIRE(interface::unerase_cast<functor_t>(f)->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo world");
    REQUIRE(interface::unerase_cast<functor_t>(f)->s_ == "hallo world");
  }
  {
    pure_functor_t ff{};
    ref_function<std::string(const std::string)> f{ff};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    move_only_function<std::string(const std::string)> f{
        std::make_unique<functor_t>("hello")};
    REQUIRE(f(" world") == "hello");
    REQUIRE(interface::unerase_cast<functor_t>(f)->s_ == "hello world");
    static_assert(!std::assignable_from<
                  move_only_function<std::string(const std::string)>,
                  move_only_function<std::string(const std::string)>>);
    move_only_function<std::string(const std::string)> f2{std::move(f)};
    REQUIRE(!has_data(get_virtual_void(f)));
    REQUIRE(f2(", bye") == "hello world");
    REQUIRE(interface::unerase_cast<functor_t>(f2)->s_ ==
            "hello world, bye");
  }
}

