#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/observer.hpp>
#include <virtual_void/shared_const.hpp>
#include <virtual_void/unique.hpp>
#include <virtual_void/any_meta_function.hpp>
#include <virtual_void/extension_method.hpp>
#include <virtual_void/virtual_typed.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void;
using namespace virtual_void;
using namespace virtual_void;


VV_V_TABLE_HAS_EXTENSION_METHODS(, test_base_i)
VV_V_TABLE_HAS_EXTENSION_METHODS(, test_derived_i)
namespace {
VV_ANY(test_base_i, (VV_CONST_METHOD(std::string, to_string)))
VV_ANY_(test_derived_i, test_base_i,
              (VV_METHOD(void, from_string, std::string const&)))

}  // namespace

namespace {
struct x_t {
  std::string s_;
};
}  // namespace

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
    extension_method_table_instance<test_base_i_v_table, x_t>();
auto derived_table =
    extension_method_table_instance<test_derived_i_v_table, x_t>();

namespace {

TEST_CASE("virtual_typed/extension_method") {
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
