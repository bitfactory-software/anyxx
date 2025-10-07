#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/virtual_typed.hpp>

#include "a.hpp"
#include "class_hierarchy_test_hierarchy.hpp"

using namespace virtual_void;
using namespace virtual_void::runtime;
using namespace virtual_void::data;
using namespace virtual_void::interface;
using namespace TestDomain;

namespace virtual_typed_test {
struct x_t {
  std::string s_;
};

VV_INTERFACE(test_i, (VV_CONST_METHOD(std::string, to_string),
                      VV_METHOD(void, from_string, std::string_view)))
}  // namespace virtual_typed_test

using namespace virtual_typed_test;

VV_RUNTIME(, int)
VV_RUNTIME(, std::string)
VV_RUNTIME_STATIC(x_t)
VV_V_TABLE_INSTANCE(, x_t, test_i)

TEST_CASE("virtual_typed/observer/base") {
  x_t s{"hallo"};

  test_i<const_observer> co{s};
  static_assert(is_interface<test_i<const_observer>>);
  CHECK(interface::unerase_cast<x_t>(co)->s_ == "hallo");
  CHECK_THROWS_AS(interface::unerase_cast<std::string>(co),
                  type_mismatch_error);

  auto co_typed = interface::as<x_t>(co);
  CHECK(co_typed->s_ == "hallo");
  CHECK((*co_typed).s_ == "hallo");
  CHECK_THROWS_AS(interface::as<std::string>(co), type_mismatch_error);

  test_i<mutable_observer> mo{s};
  CHECK(interface::unerase_cast<x_t>(mo)->s_ == "hallo");
  CHECK_THROWS_AS(interface::unerase_cast<std::string>(mo),
                  type_mismatch_error);

  auto mo_typed = interface::as<x_t>(mo);
  CHECK(mo_typed->s_ == "hallo");
  CHECK((*mo_typed).s_ == "hallo");
  (*mo_typed) = x_t{"world"};
  CHECK(mo_typed->s_ == "world");
  CHECK(co_typed->s_ == "world");
  CHECK(s.s_ == "world");
}

namespace virtual_typed_test {

template <>
struct test_i_v_table_map<x_t> {
  static auto to_string(x_t const* x) { return x->s_; }
  static void from_string(x_t* x, std::string_view s) { x->s_ = s; }
};

}  // namespace virtual_typed_test

namespace {

 template <typename X>
concept can_call_from_string = requires(X x, std::string_view s) {
  { x.from_string(s) };
};
}  // namespace

TEST_CASE("virtual_typed/observer/test_i") {
  x_t s{"hallo"};

  test_i<const_observer> co{s};
  static_assert(is_interface<test_i<const_observer>>);
  CHECK(interface::unerase_cast<x_t>(co)->s_ == "hallo");
  CHECK_THROWS_AS(interface::unerase_cast<std::string>(co),
                  type_mismatch_error);

  CHECK(co.to_string() == "hallo");

  auto co_typed = interface::as<x_t>(co);
  CHECK(co_typed->s_ == "hallo");
  CHECK((*co_typed).s_ == "hallo");
  CHECK(co_typed.to_string() == "hallo");
  CHECK_THROWS_AS(interface::as<std::string>(co), type_mismatch_error);

  test_i<mutable_observer> mo{s};
  CHECK(interface::unerase_cast<x_t>(mo)->s_ == "hallo");
  CHECK_THROWS_AS(interface::unerase_cast<std::string>(mo),
                  type_mismatch_error);

  CHECK(mo.to_string() == "hallo");

  auto mo_typed = interface::as<x_t>(mo);
  CHECK(mo_typed->s_ == "hallo");
  CHECK((*mo_typed).s_ == "hallo");
  (*mo_typed) = x_t{"world"};
  CHECK(mo_typed->s_ == "world");
  CHECK(co_typed->s_ == "world");
  CHECK(s.s_ == "world");

  mo_typed.from_string("by");
  CHECK(mo_typed->s_ == "by");
  CHECK(co_typed->s_ == "by");
  CHECK(s.s_ == "by");

  static_assert(!can_call_from_string<decltype(co_typed)>);
  static_assert(can_call_from_string<decltype(mo_typed)>);
}
