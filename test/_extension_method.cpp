#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/has_no_meta/shared_const_ptr.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/data/has_no_meta/unique_ptr.hpp>
#include <virtual_void/data/has_no_meta/value.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/extension_method.hpp>
#include <virtual_void/interface/virtual_typed.hpp>

#include "a.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::meta;
using namespace virtual_void::data::has_no_meta;
using namespace virtual_void::data;
using namespace virtual_void::interface;

namespace {
struct x_t {
  std::string s_;
};

VV_INTERFACE(test_base_i, (VV_CONST_METHOD(std::string, to_string)))
VV_INTERFACE_(test_derived_i, test_base_i,
              (VV_METHOD(void, from_string, std::string const&)))
}  // namespace

VV_RUNTIME_STATIC(type_info, x_t)

using test_base_i_co = test_base_i<const_observer>;
using test_derived_i_mo = test_derived_i<mutable_observer>;

template <>
struct test_base_i_v_table_map<x_t> {
  static auto to_string(x_t const* x) { return x->s_; }
};
template <>
struct test_derived_i_v_table_map<x_t> {
  static void from_string(x_t* x, std::string_view s) { x->s_ = s; }
};

extension_method<test_base_i_co, std::string(virtual_void::const_)>
    to_string_otherwise;
auto __ = to_string_otherwise.define<x_t>(
    [](auto expr) { return expr->s_ + " otherwise"; });

extension_method<test_derived_i_mo,
                 void(virtual_void::mutable_, std::string const&)>
    from_string_otherwise;
auto __ =
    from_string_otherwise.define<x_t>([](auto expr, std::string const& s) {
      expr->s_ = std::string{"otherwise "} + s;
    });

namespace {

TEST_CASE("virtual_typed/interface/extension_method") {
  x_t x{"hallo"};
  test_derived_i_mo i{x};
  CHECK(i.to_string() == "hallo");
  CHECK(to_string_otherwise(i) == "hallo otherwise");
}

}  // namespace
