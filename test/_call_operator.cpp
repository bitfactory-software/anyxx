#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>

#include "include/catch.hpp"

import virtual_void;
#include "virtual_void/interface/declare_macro.h"

//#include "virtual_void/data/has_no_meta/observer.h"
//#include "virtual_void/data/has_no_meta/unique_ptr.h"
//#include "virtual_void/data/has_no_meta/value.h"
//#include "virtual_void/interface/call_operator.h"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data::has_no_meta;

namespace {
struct functor_t {
  std::string s_;
  std::string operator()(const std::string& s) {
    auto ret = s_;
    s_ += s;
    return ret;
  }
  std::string operator()() const { return s_; }
};
struct functor2_t {
  std::string s_;
  std::string operator()(const std::string& s) const { return s_ + s; }
  std::string operator()() const { return s_; }
};
}  // namespace

template <typename VV>
using call1 = call_operator<std::string(), VV>;

template <typename VV>
using overloaded_function_object =
    mutable_call_operator<std::string(const std::string), VV, call1>;

template <class...>
struct make_overloaded_call_operator;

template <class SIG>
struct make_overloaded_call_operator<SIG> {
  template <virtual_void::is_virtual_void VV>
  using type = call_operator_interface<VV, base, const void*, SIG>;
};

template <class SIG, class... SIGS>
struct make_overloaded_call_operator<SIG, SIGS...> {
  template <virtual_void::is_virtual_void VV>
  using type = call_operator_interface<
      VV, typename make_overloaded_call_operator<SIGS...>::type, const void*,
      SIG>;
};

template <virtual_void::is_virtual_void VV, class... SIGS>
using overloaded_call_operator =
    make_overloaded_call_operator<SIGS...>::template type<VV>;

namespace {
template <template <typename> typename F, typename OBSERVER>
constexpr bool call_mutable = requires(functor_t functor, std::string s) {
  F<OBSERVER>{functor}(" world") == s;
};
template <template <typename> typename F, typename OBSERVER>
constexpr bool call_const = requires(functor_t const functor, std::string s) {
  F<OBSERVER>{functor}() == s;
};
}  // namespace

static_assert(!call_mutable<overloaded_function_object, const_observer>);
static_assert(call_const<overloaded_function_object, const_observer>);
static_assert(call_mutable<overloaded_function_object, mutable_observer>);
static_assert(call_const<overloaded_function_object, mutable_observer>);

TEST_CASE("call_operator overload with mutable_observer") {
  functor_t functor{"hallo"};
  overloaded_function_object<mutable_observer> f{functor};
  REQUIRE(f() == "hallo");
  REQUIRE(f(" world") == "hallo");
  REQUIRE(f() == "hallo world");
}

TEST_CASE("call_operator overload with const_observer") {
  functor_t functor{"hallo"};
  overloaded_function_object<const_observer> f{functor};
  REQUIRE(f() == "hallo");
}

TEST_CASE("make_overloaded_call_operator") {
  functor2_t functor{"hallo"};

  auto f = overloaded_call_operator<
      const_observer, std::string(const std::string& s), std::string()>{
      functor};

  REQUIRE(f() == "hallo");
  REQUIRE(f(" world") == "hallo world");
}
