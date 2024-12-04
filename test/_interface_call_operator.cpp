#include <cmath>
#include <concepts>
#include <iostream>
#include <string>
#include <vector>

#include <catch.hpp>

#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/data/has_no_meta/unique_ptr.hpp>
#include <virtual_void/data/has_no_meta/value.hpp>
#include <virtual_void/interface/call_operator.hpp>

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
using overloaded_function_object =
    overloaded_call_operator<VV, std::string(const std::string), mutable_, std::string(), const_>;

namespace {
template <template <typename> typename F, typename OBSERVER>
concept call_mutable = requires(functor_t functor, std::string s) {
  F<OBSERVER>{functor}(" world") == s;
};
template <template <typename> typename F, typename OBSERVER>
concept call_const = requires(functor_t const functor, std::string s) {
  F<OBSERVER>{functor}() == s;
};
}  // namespace

static_assert(!call_mutable<overloaded_function_object, const_observer>);
static_assert(call_const<overloaded_function_object, const_observer>);
static_assert(call_mutable<overloaded_function_object, mutable_observer>);
static_assert(!call_const<overloaded_function_object, mutable_observer>);

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
      const_observer, std::string(const std::string& s), const_, std::string(), const_>{
      functor};

  REQUIRE(f() == "hallo");
  REQUIRE(f(" world") == "hallo world");
}
