#include "component_base.hpp"

#include <bit_factory/anyxx.hpp>

using namespace test::component_base;

using namespace anyxx;

namespace test::component_base {

struct X {
  double d_;
  [[nodiscard]] std::string to_string() const { return std::to_string(d_); }
  [[nodiscard]] double get_value() const { return d_; }
  void set_value(double d) { d_ = d; }
};

static X a_x{3.14};

}  // namespace test::component_base

using namespace test::component_base;

ANY_MODEL_FWD(COMPONENT_BASE_EXPORT, test::component_base::X,
              test::component_base, get_value_i)
// ANY_MODEL_FWD(COMPONENT_BASE_EXPORT, test::component_base::X,
//               test::component_base, set_value_i)
ANY_MODEL_FWD(COMPONENT_BASE_EXPORT, test::component_base::X,
              test::component_base, to_string_i)
//
ANY_META_CLASS_STATIC(X)
ANY_MODEL(X, test::component_base, get_value_i)
ANY_MODEL(X, test::component_base, set_value_i)
// ANY_REGISTER_MODEL(X, test::component_base::set_value_i)
ANY_MODEL(X, test::component_base, to_string_i)

to_string_i<anyxx::const_observer> test::component_base::get_to_string_i_co() {
  to_string_i<anyxx::const_observer> i{a_x};
  auto meta = get_v_table(i)->meta_data_;
  [[maybe_unused]] const std::type_info* type_info = &meta->get_type_info();
  static_assert(is_any<to_string_i<anyxx::const_observer>>);
  [[maybe_unused]] const std::type_info* type_info_1 = &typeid(X);
  assert(type_info == type_info_1);
  [[maybe_unused]] auto s = i.to_string();
  return i;
}
to_string_i<anyxx::shared_const> test::component_base::get_to_string_i_sc(
    double v) {
  return std::make_shared<X>(v);
}
to_string_i<anyxx::unique> test::component_base::get_to_string_i_u(double v) {
  return std::make_unique<X>(v);
}

anyxx::any<anyxx::shared_const> test::component_base::sc_X(double v) {
  return {std::make_shared<X>(v)};
}
anyxx::any<anyxx::unique> test::component_base::u_X(double v) {
  return {anyxx::any<unique>(std::make_unique<X>(v))};
}
