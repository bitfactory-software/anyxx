#include "component_base.hpp"

#include <assert.h>

#include <virtual_void/data/has_type_info/typeid_of.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/cast_has_type_info.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace test::component_base;

using namespace virtual_void;
using namespace virtual_void::interface;
using namespace virtual_void::data::has_type_info;

namespace test::component_base {

struct X {
  double d_;
  std::string to_string() const { return std::to_string(d_); }
  double get_value() const { return d_; }
  void set_value(double d) { d_ = d; }
};

static X x{3.14};

}  // namespace test::component_base

void test::component_base::enable_casts() {
  enable_attach_interface<get_value_i, X, const_observer>();
  assert((find_copy<const_observer, const_observer>().is_defined<X>()));
  enable_attach_interface<get_value_i, X, mutable_observer>();
  enable_attach_interface<get_value_i, X, shared_const>();
  enable_attach_interface<get_value_i, X, unique>();
  enable_attach_interface<get_value_i, X, unique, shared_const>();
  enable_attach_interface<set_value_i, X, unique, shared_const>();
  enable_attach_interface<to_string_i, X, const_observer, unique>();
  enable_attach_interface<set_value_i, X, mutable_observer, unique>();

  enable_move_to_interface<to_string_i, X, unique, unique>();
  assert((find_move<unique, unique>().is_defined<X>()));
  enable_move_to_interface<get_value_i, X, shared_const, unique>();
}

to_string_i<virtual_void::data::has_type_info::const_observer>
test::component_base::get_to_string_i_co() {
  auto p = &x;
  to_string_i<virtual_void::data::has_type_info::const_observer> i{x};
  auto meta = get_meta(get_virtual_void(i));
  const std::type_info* type_info = meta->type_info();
  const std::type_info* type_info_1 = &typeid(X);
  assert(type_info == type_info_1);
  auto s = i.to_string();
  return i;
}
to_string_i<virtual_void::data::has_type_info::shared_const>
test::component_base::get_to_string_i_sc(double v) {
  return X{v};
}
to_string_i<virtual_void::data::has_type_info::unique>
test::component_base::get_to_string_i_u(double v) {
  return X{v};
}

virtual_void::data::has_type_info::shared_const test::component_base::sc_X(
    double v) {
  return virtual_void::erased<shared_const>(X{v});
}
virtual_void::data::has_type_info::unique test::component_base::u_X(double v) {
  return virtual_void::erased<unique>(X{v});
}

VV_TYPEID_OF_DEFINE(test::component_base::to_string_i<
                    virtual_void::data::has_type_info::const_observer>);
VV_TYPEID_OF_DEFINE(test::component_base::to_string_i<
                    virtual_void::data::has_type_info::unique>);

VV_TYPEID_OF_DEFINE(test::component_base::get_value_i<
                    virtual_void::data::has_type_info::const_observer>);
VV_TYPEID_OF_DEFINE(test::component_base::get_value_i<
                    virtual_void::data::has_type_info::mutable_observer>);
VV_TYPEID_OF_DEFINE(test::component_base::get_value_i<
                    virtual_void::data::has_type_info::shared_const>);
VV_TYPEID_OF_DEFINE(test::component_base::get_value_i<
                    virtual_void::data::has_type_info::unique>);

VV_TYPEID_OF_DEFINE(test::component_base::set_value_i<
                    virtual_void::data::has_type_info::mutable_observer>);
VV_TYPEID_OF_DEFINE(test::component_base::set_value_i<
                    virtual_void::data::has_type_info::unique>);
