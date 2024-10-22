#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/interface.h"
#include "../include/virtual_void/erased/lifetime/cast.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;

const double M_PI = 3.14;

struct position {
  float x, y;
};

struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

ERASED_INTERFACE(to_string_i, (INTERFACE_CONST_METHOD(std::string, to_string)))

using to_string_sc = to_string_i<erased::shared_const>;
using to_string_co = to_string_i<erased::const_observer>;

namespace virtual_void::erased {
template <typename ERASED_TO>
auto v_table_cast(const auto &v_table) {
  static_assert(
      std::same_as<to_string_co::interface_t, to_string_sc::interface_t>);
  return static_cast<typename ERASED_TO::interface_t *>(v_table);
}
}  // namespace virtual_void::erased

TEST_CASE("interface lifetime cast") {
  to_string_sc sc{X{"hallo"}};
  REQUIRE(sc.to_string() == "hallo");
  REQUIRE(sc.is_derived_from<erased::base<erased::shared_const>>());

  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void const *>>, void>);
  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void *>>, void>);
  // static_assert( std::same_as<std::decay_t<void const *>,
  // std::add_const_t<void*>);

  auto o1 = lifetime_cast<erased::const_observer>(sc.get_lifetime_holder());
  auto x = erased::reconcrete_cast<X>(o1);
  auto x1 = static_cast<X const *>(sc.get_lifetime_holder().data());
  REQUIRE(x->s_ == "hallo");

  to_string_co co = interface_lifetime_cast<to_string_co>(sc);
  REQUIRE(co.to_string() == "hallo");
  static_assert(
      std::same_as<to_string_co::interface_t, to_string_sc::interface_t>);
  REQUIRE(co.is_derived_from<erased::base<erased::const_observer>>());
}