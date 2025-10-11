#include "component_base.hpp"

#include <assert.h>

#include <virtual_void/interface/base.hpp>

using namespace test::component_base;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::runtime;
using namespace virtual_void::data;

namespace test::component_base {

struct X {
  double d_;
  std::string to_string() const { return std::to_string(d_); }
  double get_value() const { return d_; }
  void set_value(double d) { d_ = d; }
};

static X x{3.14};

}  // namespace test::component_base

using namespace test::component_base;

// template <>
// struct COMPONENT_BASE_EXPORT get_value_i_v_table_instance<X> {
//   static get_value_i_v_table* get() {
//     static get_value_i_v_table v_table{std::in_place_type<X>};
//     return &v_table;
//   };
// };
// template <>
// struct COMPONENT_BASE_EXPORT to_string_i_v_table_instance<X> {
//   static to_string_i_v_table* get() {
//     static to_string_i_v_table v_table{std::in_place_type<X>};
//     return &v_table;
//   };
// };

VV_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        get_value_i)
VV_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        set_value_i)
VV_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        to_string_i)

VV_RUNTIME_STATIC(X)
VV_V_TABLE_INSTANCE(X, test::component_base, get_value_i)
VV_V_TABLE_INSTANCE(X, test::component_base, set_value_i)
VV_V_TABLE_INSTANCE(X, test::component_base, to_string_i)

to_string_i<virtual_void::data::const_observer>
test::component_base::get_to_string_i_co() {
  auto p = &x;
  to_string_i<virtual_void::data::const_observer> i{x};
  auto meta = get_v_table(i)->meta_data;
  const std::type_info* type_info = &meta->get_type_info();
  static_assert(
      interface::is_interface<to_string_i<virtual_void::data::const_observer>>);
  const std::type_info* type_info_1 = &typeid(X);
  assert(type_info == type_info_1);
  auto s = i.to_string();
  return i;
}
to_string_i<virtual_void::data::shared_const>
test::component_base::get_to_string_i_sc(double v) {
  return std::make_shared<X>(v);
}
to_string_i<virtual_void::data::unique> test::component_base::get_to_string_i_u(
    double v) {
  return std::make_unique<X>(v);
}

virtual_void::data::shared_const test::component_base::sc_X(double v) {
  return virtual_void::data::erased<shared_const>(std::make_shared<X>(v));
}
virtual_void::data::unique test::component_base::u_X(double v) {
  return virtual_void::data::erased<unique>(std::make_unique<X>(v));
}
