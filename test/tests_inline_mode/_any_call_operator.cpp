#include <catch.hpp>
#include <string>
#include <anyxx/anyxx.hpp>

using namespace Catch::Matchers;

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
ANY(overloaded_function_object,
             (ANY_OP(std::string, (), (std::string const&)),
              ANY_CONST_OP(std::string, ())))
}

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

// muy complicato... feature extravaganza... can be done, but is it worth the
// time?
// TEST_CASE("call_operator overload with const_observer") {
//  functor_t functor{"hallo"};
//  overloaded_function_object<const_observer> f{functor};
//  REQUIRE(f() == "hallo");
//}

TEST_CASE("make_overloaded_call_operator") {
  functor2_t functor{"hallo"};

  auto f = overloaded_function_object<const_observer>{functor};
  REQUIRE(f() == "hallo");
}
