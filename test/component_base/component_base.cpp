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

using namespace anyxx;
using namespace test::component_base;

//
META_CLASS_STATIC(X)
namespace {
enable_dynamic<X, test::component_base::get_value_i> __;
enable_dynamic<X, test::component_base::set_value_i> __;
enable_dynamic<X, test::component_base::to_string_i> __;
}  // namespace

any_<anyxx::cref, to_string_i> test::component_base::get_to_string_i_co() {
  any_<cref, to_string_i> i{a_x};
  auto meta = get_v_table(i)->meta_data_;
  [[maybe_unused]] const std::type_info* type_info = &meta->type();  // NOLINT
  static_assert(is_any<any_<cref, to_string_i>>);
  [[maybe_unused]] const std::type_info* type_info_1 = &typeid(X);
  assert(type_info == type_info_1);
  [[maybe_unused]] auto s = i.to_string();
  return i;
}
any_<anyxx::shared, to_string_i> test::component_base::get_to_string_i_sc(
    double v) {
  return std::make_shared<X>(v);
}
any_<anyxx::unique, to_string_i> test::component_base::get_to_string_i_u(
    double v) {
  return std::make_unique<X>(v);
}

anyxx::any_<anyxx::shared, anyxx::dynamic_deletable> test::component_base::sc_X(
    double v) {
  return {std::make_shared<X>(v)};
}
anyxx::any_<anyxx::unique, anyxx::dynamic_deletable> test::component_base::u_X(
    double v) {
  return {anyxx::any_<anyxx::unique, anyxx::dynamic_deletable>(
      std::make_unique<X>(v))};
}
