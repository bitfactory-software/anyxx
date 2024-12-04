#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>

 #include <virtual_void/data/has_no_meta/observer.hpp>
 #include <virtual_void/data/has_no_meta/unique_ptr.hpp>
 #include <virtual_void/data/has_no_meta/value.hpp>
 #include <virtual_void/interface/call_operator.hpp>
 #include <virtual_void/interface/declare_macro.hpp>

#include <catch.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;

namespace virtual_void {
template <typename SIG>
using function = interface::call_operator<data::has_no_meta::value, SIG>;
template <typename SIG>
using ref_function =
    interface::call_operator<data::has_no_meta::mutable_observer, SIG>;
template <typename SIG>
using move_only_function =
    interface::call_operator<data::has_no_meta::unique_ptr, SIG>;
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

TEST_CASE("std emulated function") {
  {
    functor_t functor{"hallo"};
    function<std::string(const std::string)> f{functor};
    data::has_no_meta::meta const* m = get_meta(get_virtual_void(f));
    REQUIRE(unerase_cast<functor_t>(get_virtual_void(f))->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo");
    REQUIRE(unerase_cast<functor_t>(get_virtual_void(f))->s_ == "hallo world");
  }
  {
    function<std::string(const std::string)> f{[](auto s) { return s; }};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    functor_t functor{"hallo"};
    ref_function<std::string(const std::string)> f{functor};
    REQUIRE(unerase_cast<functor_t>(get_virtual_void(f))->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo world");
    REQUIRE(unerase_cast<functor_t>(get_virtual_void(f))->s_ == "hallo world");
  }
  {
    auto func = [](auto s) { return s; };
    ref_function<std::string(const std::string)> f{func};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    move_only_function<std::string(const std::string)> f{
        std::make_unique<functor_t>("hello")};
    REQUIRE(f(" world") == "hello");
    REQUIRE(unerase_cast<functor_t>(get_virtual_void(f))->s_ == "hello world");
    static_assert(!std::assignable_from<
                  move_only_function<std::string(const std::string)>,
                  move_only_function<std::string(const std::string)>>);
    move_only_function<std::string(const std::string)> f2 { std::move(f) };
    REQUIRE(!has_data(get_virtual_void(f)));
    REQUIRE(f2(", bye") == "hello world");
    REQUIRE(unerase_cast<functor_t>(get_virtual_void(f2))->s_ ==
            "hello world, bye");
  }
}
}  // namespace
