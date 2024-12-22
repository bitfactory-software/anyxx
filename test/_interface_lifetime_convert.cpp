#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/data/has_no_meta/shared_const.hpp>
#include <virtual_void/data/has_no_meta/unique.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/declare_macro.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;

struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

VV_INTERFACE(to_string_i, (VV_CONST_METHOD(std::string, to_string)))

using to_string_sc = to_string_i<data::has_no_meta::shared_const>;
using to_string_co = to_string_i<data::has_no_meta::const_observer>;

using to_string_u = to_string_i<data::has_no_meta::unique>;
using to_string_mo = to_string_i<data::has_no_meta::mutable_observer>;

TEST_CASE("interface lifetime cast") {
  to_string_sc sc{X{"hallo"}};
  REQUIRE(sc.to_string() == "hallo");
  REQUIRE(
      is_derived_from<interface::base<data::has_no_meta::shared_const>>(sc));

  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void const *>>, void>);
  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void *>>, void>);
  // static_assert( std::same_as<std::decay_t<void const *>,
  // std::add_const_t<void*>);

  auto o1 = get_virtual_void(sc);
  auto x = unerase_cast<X>(o1);
  auto x1 = static_cast<X const *>(get_data(get_virtual_void(sc)));
  REQUIRE(x->s_ == "hallo");

  to_string_co co = sc;
  REQUIRE(co.to_string() == "hallo");
  static_assert(std::same_as<to_string_co::v_table_t, to_string_sc::v_table_t>);
  REQUIRE(
      is_derived_from<interface::base<data::has_no_meta::const_observer>>(co));

  to_string_u u{X{"hallo"}};
  REQUIRE(u.to_string() == "hallo");
  to_string_mo mo{u};
  REQUIRE(mo.to_string() == "hallo");


  to_string_u u1{X{"hallo"}};
  REQUIRE(u1.to_string() == "hallo");
  to_string_co co_from_u{u1};
  REQUIRE(co_from_u.to_string() == "hallo");
}