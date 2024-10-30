#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "virtual_void/data/has_no_meta/observer.h"
#include "virtual_void/data/has_no_meta/value.h"
#include "virtual_void/interface/call_operator.h"
#include "virtual_void/interface/declare_macro.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

namespace virtual_void {
template <typename SIG>
using function = interface::call_operator<SIG, data::has_no_meta::value>;
template <typename SIG>
using ref_function =
    interface::call_operator<SIG, data::has_no_meta::mutable_observer>;
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
    REQUIRE(reconcrete_cast<functor_t>(*f)->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo");
    REQUIRE(reconcrete_cast<functor_t>(*f)->s_ == "hallo world");
  }
  {
    function<std::string(const std::string)> f{
        [](auto s) { return s; }};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    functor_t functor{"hallo"};
    ref_function<std::string(const std::string)> f{functor};
    REQUIRE(reconcrete_cast<functor_t>(*f)->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo world");
    REQUIRE(reconcrete_cast<functor_t>(*f)->s_ == "hallo world");
  }
  {
    auto func = [](auto s) { return s; };
    ref_function<std::string(const std::string)> f{func};
    REQUIRE(f("hello world") == "hello world");
  }
}
}  // namespace
