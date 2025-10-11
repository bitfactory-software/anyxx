#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/extension_method.hpp>
#include <virtual_void/interface/virtual_typed.hpp>

#include "a.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::runtime;
using namespace virtual_void::data;
using namespace virtual_void::interface;

namespace {
struct x_t {
  std::string s_;
};
}  // namespace
VV_V_TABLE_HAS_EXTENSION_METHODS(, test_base_i)
VV_V_TABLE_HAS_EXTENSION_METHODS(, test_derived_i)

namespace {

VV_INTERFACE(test_base_i, (VV_CONST_METHOD(std::string, to_string)))
VV_INTERFACE_(test_derived_i, test_base_i,
              (VV_METHOD(void, from_string, std::string const&)))

}  // namespace

VV_EXTENSION_METHOD_COUNT(test_base_i)
VV_EXTENSION_METHOD_COUNT(test_derived_i)

VV_RUNTIME_STATIC(x_t)
VV_EXTENSION_TABLE_INSTANCE(x_t, test_base_i)
VV_EXTENSION_TABLE_INSTANCE(x_t, test_derived_i)

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

extension_method<std::string(virtual_<test_base_i_co>)>
    to_string_otherwise;
auto __ = to_string_otherwise.define<x_t>(
    [](auto expr) { return expr->s_ + " otherwise"; });

extension_method<
                 void(virtual_<test_derived_i_mo>, std::string const&)>
    from_string_otherwise;
auto __ =
    from_string_otherwise.define<x_t>([](auto expr, std::string const& s) {
      expr->s_ = std::string{"otherwise "} + s;
    });

auto base_table =
    runtime::extension_method_table_instance<test_base_i_v_table, x_t>();
auto derived_table =
    runtime::extension_method_table_instance<test_derived_i_v_table, x_t>();

namespace {

TEST_CASE("virtual_typed/interface/extension_method") {
  CHECK(base_table->size() == 1);
  CHECK(derived_table->size() == 1);

  CHECK(test_base_i_v_table::imlpementation<x_t>()
            ->own_extension_method_holder_t::extension_method_table);
  CHECK(test_derived_i_v_table::imlpementation<x_t>()
            ->own_extension_method_holder_t::extension_method_table);

  x_t x{"hallo"};
  test_derived_i_mo i{x};
  CHECK(i.to_string() == "hallo");
  CHECK(to_string_otherwise(i) == "hallo otherwise");
}

}  // namespace
