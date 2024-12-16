#include "component_base.hpp"

#include <assert.h>

#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/i_table.hpp>

using namespace test::component_base;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data::has_i_table;


namespace test::component_base {

struct X {
  double d_;
  std::string to_string() const { return std::to_string(d_); }
  double get_value() const { return d_; }
  void set_value(double d) { d_ = d; }
};

static X x{3.14};

}  // namespace test::component_base

VV_IS_A_CONST(X, get_value_i);
VV_IS_A_MUTABLE(X, set_value_i);
VV_IS_A_CONST(X, to_string_i);

to_string_i<virtual_void::data::has_i_table::const_observer>
test::component_base::get_to_string_i_co() {
  auto p = &x;
  to_string_i<virtual_void::data::has_i_table::const_observer> i{x};
  auto meta = get_meta(get_virtual_void(i));
  const std::type_info* type_info = meta->type_info();
  const std::type_info* type_info_1 = &typeid(X);
  assert(type_info == type_info_1);
  auto s = i.to_string();
  return i;
}
to_string_i<virtual_void::data::has_i_table::shared_const>
test::component_base::get_to_string_i_sc(double v) {
  return X{v};
}
to_string_i<virtual_void::data::has_i_table::unique>
test::component_base::get_to_string_i_u(double v) {
  return X{v};
}

virtual_void::data::has_i_table::shared_const test::component_base::sc_X(
    double v) {
  return virtual_void::erased<shared_const>(X{v});
}
virtual_void::data::has_i_table::unique test::component_base::u_X(double v) {
  return virtual_void::erased<unique>(X{v});
}

VV_DEFINE_V_TABLE_INDEX(test::component_base::get_value_i)
VV_DEFINE_V_TABLE_INDEX(test::component_base::set_value_i)
VV_DEFINE_V_TABLE_INDEX(test::component_base::to_string_i)
