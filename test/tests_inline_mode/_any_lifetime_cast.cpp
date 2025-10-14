#include <catch.hpp>
#include <string>
#include <anyxx/anyxx.hpp>

using namespace Catch::Matchers;

using namespace anyxx;

namespace {
struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

ANY(to_string_i, (ANY_CONST_METHOD(std::string, to_string)))

using to_string_sc = to_string_i<shared_const>;
using to_string_co = to_string_i<const_observer>;

using to_string_u = to_string_i<unique>;
using to_string_mo = to_string_i<mutable_observer>;
}  // namespace

TEST_CASE("any lifetime cast") {
  to_string_sc sc{std::make_shared<X>("hallo")};
  REQUIRE(sc.to_string() == "hallo");
  REQUIRE(
      is_derived_from<any_base<shared_const>>(sc));

  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void const *>>, void>);
  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void *>>, void>);
  // static_assert( std::same_as<std::decay_t<void const *>,
  // std::add_const_t<void*>);

  auto o1 = get_erased_data(sc);
  auto x = unerase_cast<X>(sc);
  auto x1 = static_cast<X const *>(get_void_data_ptr(sc));
  REQUIRE(x->s_ == "hallo");

  static_assert(std::same_as<to_string_co::v_table_t, to_string_sc::v_table_t>);
  static_assert(
      std::derived_from<to_string_sc::v_table_t, to_string_co::v_table_t>);
  to_string_co co = sc;
  REQUIRE(co.to_string() == "hallo");
  static_assert(std::same_as<to_string_co::v_table_t, to_string_sc::v_table_t>);
  REQUIRE(
      is_derived_from<any_base<const_observer>>(co));

  to_string_u u{ std::make_unique<X>("hallo") };
  REQUIRE(u.to_string() == "hallo");
  static_assert(!is_typed_any<to_string_u>);
  static_assert(is_any<to_string_u>);
  static_assert(!constructibile_for<to_string_u, to_string_mo::erased_data_t>);
  static_assert(std::derived_from<to_string_mo::v_table_t, to_string_u::v_table_t>);
  to_string_mo mo{u};
  REQUIRE(mo.to_string() == "hallo");

  to_string_u u1{ std::make_unique<X>("hallo") };
  REQUIRE(u1.to_string() == "hallo");
  to_string_co co_from_u{u1};
  REQUIRE(co_from_u.to_string() == "hallo");
}