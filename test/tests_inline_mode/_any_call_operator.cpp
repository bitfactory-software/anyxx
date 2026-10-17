#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace anyxx;
using namespace anyxx;
using namespace anyxx;

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

namespace {
TRAIT(overloaded_function_object, anyxx::dynamic_copyable,
      (OP_(std::string, (), (std::string const&), ),
       OP_(std::string, (), (), const)))
}

namespace {
template <typename Any>
concept call_mutable =
    requires(functor_t functor, std::string s, Any a) { a(" world") == s; };
template <template <typename> typename F, typename OBSERVER>
concept call_const = requires(functor_t const functor, std::string s) {
  any<OBSERVER, F>{functor}() == s;
};
}  // namespace

static_assert(!call_mutable<any<cref, overloaded_function_object>>);
static_assert(call_const<overloaded_function_object, cref>);
static_assert(call_mutable<any<mutref, overloaded_function_object>>);
static_assert(!call_const<overloaded_function_object, mutref>);

TEST_CASE("call_operator overload with mutref") {
  functor_t functor{"hallo"};
  any<mutref, overloaded_function_object> f{functor};
  REQUIRE(f() == "hallo");
  REQUIRE(f(" world") == "hallo");
  REQUIRE(f() == "hallo world");
}

// muy complicato... feature extravaganza... can be done, but is it worth the
// time?
// TEST_CASE("call_operator overload with cref") {
//  functor_t functor{"hallo"};
//  overloaded_function_object<cref> f{functor};
//  REQUIRE(f() == "hallo");
//}

TEST_CASE("make_overloaded_call_operator") {
  functor2_t functor{"hallo"};

  auto f = any<cref, overloaded_function_object>{functor};
  REQUIRE(f() == "hallo");
}
