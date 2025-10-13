#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <anypp/observer.hpp>
#include <anypp/shared_const.hpp>
#include <anypp/unique.hpp>
#include <anypp/any_base.hpp>
#include <anypp/any_meta_function.hpp>
#include <anypp/meta_data.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;

namespace {
template <typename T>
struct test_trait;
template <>
struct test_trait<const void*> {
  static const bool is_const = true;
};
template <>
struct test_trait<void*> {
  static const bool is_const = false;
};
template <typename VV_VOID>
struct test_interface {
  VV_VOID member = nullptr;
  std::string f(int)
    requires(!test_trait<VV_VOID>::is_const)
  {
    return "mutable";
  }
  std::string f(int) const { return "const"; }
};
}  // namespace
TEST_CASE("_interface_const_correct prototyping") {
  using namespace virtual_void;
  static_assert(!test_trait<void*>::is_const);
  static_assert(trait<const_observer>::is_const);
  test_interface<void const*> i1;
  REQUIRE(i1.f(1) == "const");

  test_interface<void const*> const i2;
  REQUIRE(i2.f(1) == "const");

  test_interface<void*> i3;
  REQUIRE(i3.f(1) == "mutable");

  test_interface<void*> const i4;
  REQUIRE(i4.f(1) == "const");
}

namespace {
struct functor {
  std::string text = "hallo";
  std::string operator()() const { return text; }
  void operator()(const std::string& t) { text = t; }
};
}  // namespace

namespace {

VV_ANY(const_function_i, (VV_CONST_OP(std::string, ())))
VV_ANY(mutating_function_i, (VV_OP(void, (), std::string const&)))

using const_function = const_function_i<const_observer>;
using mutating_function = mutating_function_i<mutable_observer>;
using shared_const_function = const_function_i<shared_const>;
using shared_mutating_function = mutating_function_i<shared_const>;

static_assert(std::is_constructible_v<const_function, functor const>);
 static_assert(std::is_assignable_v<const_function,
                                       const_function>);
static_assert(std::is_assignable_v<const_function,
                                    functor const>); 
static_assert(!std::is_constructible_v<mutating_function,
                                       functor const>);  // <- may not compile!
static_assert(!std::is_assignable_v<mutating_function,
                                    const_function>);  // <- may not compile!
static_assert(
    !std::is_assignable_v<mutating_function,
                          const_function const>);  // <- may not compile!
static_assert(!std::is_assignable_v<mutating_function,
                                    functor const>);  // <- may not compile!
static_assert(!std::is_assignable_v<mutating_function const,
                                    functor const>);  // <- may not compile!

}  // namespace


TEST_CASE("_interface_const_correct const/mutable_obseerver call operator") {
  using namespace virtual_void;

  {
    functor function_object;
    const_function cf = function_object;
    mutating_function mf = function_object;
    REQUIRE(cf() == "hallo");
    mf("world");
    REQUIRE(cf() == "world");
  }

  {
    functor function_object;
    const_function const cf = function_object;
    mutating_function const mf = function_object;
    REQUIRE(cf() == "hallo");
    mf("world");
    REQUIRE(cf() == "world");
  }

  {
    functor const const_function_object;
    const_function cf = const_function_object;
    REQUIRE(cf() == "hallo");
    const_function cf2{cf};
    // cf2 = cf; //should compile
  }

  {
    functor const function_object;
    const_function const cf = function_object;
    REQUIRE(cf() == "hallo");
  }

  {
    functor function_object;
    const_function cf = function_object;
    REQUIRE(cf() == "hallo");
  }

  {
    functor function_object;
    const_function const cf = function_object;
    REQUIRE(cf() == "hallo");
  }

  {
    functor const const_function_object;
    const_function cf = const_function_object;
    REQUIRE(cf() == "hallo");
  }

  {
    functor const function_object;
    const_function const cf = function_object;
    REQUIRE(cf() == "hallo");
  }
}

TEST_CASE("_interface_const_correct virtual_void::shared_const") {

  {
    shared_const_function cf = std::make_shared<functor>();
    static_assert(
        !std::is_assignable_v<shared_mutating_function,
                              shared_const_function>);  // <- may not compile!
    REQUIRE(cf() == "hallo");
  }

  {
    shared_const_function const cf = std::make_shared<functor>();
    static_assert(
        !std::is_assignable_v<shared_mutating_function,
                              const_function const>);  // <- may not compile!
    REQUIRE(cf() == "hallo");
  }

  {
    shared_const_function cf = std::make_shared<functor const>();
    static_assert(std::is_assignable_v<shared_mutating_function,
                                        functor const>);
    REQUIRE(cf() == "hallo");
  }

  {
    shared_const_function const cf = std::make_shared<functor const>();
    static_assert(!std::is_assignable_v<shared_mutating_function const,
                                        functor const>);  // <- may not compile!
    REQUIRE(cf() == "hallo");
  }
}

namespace {
struct text_object {
  std::string text = "hallo";
  std::string get_text() const { return text; }
  void set_text(std::string const& t) { text = t; }
};

VV_ANY(text_i_const, (VV_CONST_METHOD(std::string, get_text)))

VV_ANY_(text_i_mutable, text_i_const,
              (VV_METHOD(void, set_text, std::string const&)))
}  // namespace

using const_text_i = text_i_const<const_observer>;
using const_text_i_mutable = text_i_const<mutable_observer>;

template <class C>
concept can_call_get_text = requires(C c) {
  { c.get_text() } -> std::same_as<std::string>;
};
template <class C>
concept can_call_set_text = requires(C c, std::string s) {
  { c.set_text(s) };
};

static_assert(std::constructible_from<const_text_i, text_object>);
static_assert(std::constructible_from<const_text_i, text_object const>);
static_assert(can_call_get_text<const_text_i>);
static_assert(can_call_get_text<const_text_i const>);
static_assert(!can_call_set_text<const_text_i>);
static_assert(!can_call_set_text<const_text_i const>);

using mutable_text_i_const = text_i_mutable<const_observer>;
using mutable_text_i_mutable = text_i_mutable<mutable_observer>;
static_assert(std::same_as<mutable_text_i_mutable::erased_data_t,
                           mutable_observer>);

static_assert(!std::is_const_v<std::remove_reference_t<text_object&&>>);
static_assert(std::is_const_v<std::remove_reference_t<text_object const&&>>);
using void_const = trait<virtual_void::const_observer>::void_t;
static_assert(is_const_void<void_const>);
using void_mutable = trait<virtual_void::mutable_observer>::void_t;
static_assert(!is_const_void<void_mutable>);
static_assert(
    !(!std::is_const_v<std::remove_reference_t<text_object const&&>> ||
      is_const_void<void_mutable>));
static_assert(!std::is_const_v<std::remove_reference_t<text_object const&&>> ||
              is_const_void<void_const>);
static_assert(trait<const_observer>::is_const);
static_assert(!trait<mutable_observer>::is_const);

static_assert(std::constructible_from<mutable_text_i_mutable, text_object>);
static_assert(
    !std::constructible_from<mutable_text_i_mutable, text_object const>);
static_assert(can_call_get_text<mutable_text_i_mutable>);
static_assert(can_call_get_text<mutable_text_i_mutable const>);
static_assert(can_call_set_text<mutable_text_i_mutable>);
static_assert(can_call_set_text<mutable_text_i_mutable const>);

TEST_CASE("_interface_const_correct const/mutable member function") {
  using namespace virtual_void;

  text_object a_text;
  const text_object a_text_const;
  const_text_i const const_const_i1{a_text};
  const_text_i const_i2{a_text};
  static_assert(std::is_const_v<decltype(a_text_const)> ||
                is_const_void_<void_mutable>::value);
  // mutable_text_i const_mutable_i1{a_text_const};  // may not compile
  mutable_text_i_mutable const const_mutable_i2{a_text};
  mutable_text_i_mutable mutable_i1{a_text};

  REQUIRE(const_text_i(a_text).get_text() == "hallo");
  REQUIRE(const_text_i(a_text_const).get_text() == "hallo");
  REQUIRE(const_const_i1.get_text() == "hallo");
  REQUIRE(const_i2.get_text() == "hallo");

  //  REQUIRE(mutable_text_i(a_text_const).get_text() ==
  //          "hallo");                                // may not compile!
  //  mutable_text_i(a_text_const).set_text("hallo");  // may not compile!
  REQUIRE(mutable_text_i_mutable(a_text).get_text() == "hallo");
  //  REQUIRE(mutable_text_i(a_text_const).get_text() == "hallo"); // may not
  //  compile!
  mutable_text_i_mutable(a_text).set_text("world");
  REQUIRE(mutable_text_i_mutable(a_text).get_text() == "world");
  const_mutable_i2.set_text("again");
  REQUIRE(const_mutable_i2.get_text() == "again");
}
