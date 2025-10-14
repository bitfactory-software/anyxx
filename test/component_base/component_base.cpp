#include "component_base.hpp"
#include <anyxx/anyxx.hpp>

using namespace test::component_base;

using namespace anyxx;

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

ANY_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        get_value_i)
ANY_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        set_value_i)
ANY_V_TABLE_INSTANCE_FWD(COMPONENT_BASE_EXPORT, test::component_base::X, test::component_base,
                        to_string_i)

ANY_META_CLASS_STATIC(X)
ANY_V_TABLE_INSTANCE(X, test::component_base, get_value_i)
ANY_V_TABLE_INSTANCE(X, test::component_base, set_value_i)
ANY_V_TABLE_INSTANCE(X, test::component_base, to_string_i)

to_string_i<anyxx::const_observer>
test::component_base::get_to_string_i_co() {
  auto p = &x;
  to_string_i<anyxx::const_observer> i{x};
  auto meta = get_v_table(i)->meta_data;
  const std::type_info* type_info = &meta->get_type_info();
  static_assert(
      is_any<to_string_i<anyxx::const_observer>>);
  const std::type_info* type_info_1 = &typeid(X);
  assert(type_info == type_info_1);
  auto s = i.to_string();
  return i;
}
to_string_i<anyxx::shared_const>
test::component_base::get_to_string_i_sc(double v) {
  return std::make_shared<X>(v);
}
to_string_i<anyxx::unique> test::component_base::get_to_string_i_u(
    double v) {
  return std::make_unique<X>(v);
}

anyxx::shared_const test::component_base::sc_X(double v) {
  return anyxx::erased<shared_const>(std::make_shared<X>(v));
}
anyxx::unique test::component_base::u_X(double v) {
  return anyxx::erased<unique>(std::make_unique<X>(v));
}
