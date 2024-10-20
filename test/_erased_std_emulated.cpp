#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/interface.h"
#include "../include/virtual_void/erased/call_operator.h"
#include "../include/virtual_void/erased/lifetime/observer.h"
#include "../include/virtual_void/erased/lifetime/value.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

namespace virtual_void::erased {
template <typename SIG>
using function =
    virtual_void::erased::call_operator<SIG, erased::value,
                                        virtual_void::erased::base>;
template <typename SIG>
using ref_function =
    virtual_void::erased::call_operator<SIG, erased::mutable_observer<>,
                                        virtual_void::erased::base>;
}  // namespace virtual_void::erased

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

TEST_CASE("erased std emulated function") {
  {
    functor_t functor{"hallo"};
    erased::function<std::string(const std::string)> f{functor};
    REQUIRE(reconcrete_cast<functor_t>(f.get_lifetime_holder())->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo");
    REQUIRE(reconcrete_cast<functor_t>(f.get_lifetime_holder())->s_ ==
            "hallo world");
  }
  {
    erased::function<std::string(const std::string)> f{
        [](auto s) { return s; }};
    REQUIRE(f("hello world") == "hello world");
  }
  {
    functor_t functor{"hallo"};
    erased::ref_function<std::string(const std::string)> f{functor};
    REQUIRE(reconcrete_cast<functor_t>(f.get_lifetime_holder())->s_ == "hallo");
    REQUIRE(f(" world") == "hallo");
    REQUIRE(functor.s_ == "hallo world");
    REQUIRE(reconcrete_cast<functor_t>(f.get_lifetime_holder())->s_ ==
            "hallo world");
  }
  {
    auto func = [](auto s) { return s; };
    erased::ref_function<std::string(const std::string)> f{func};
    REQUIRE(f("hello world") == "hello world");
  }
}
}  // namespace
