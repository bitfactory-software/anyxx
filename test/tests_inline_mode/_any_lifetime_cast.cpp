#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace anyxx;

static_assert(anyxx::is_in_dll_mode == false);

namespace {
struct X {
  std::string s_;
  [[nodiscard]] std::string to_string() const { return s_; }
};

ANY(to_string_i, (ANY_METHOD(std::string, to_string, (), const)), , )
using to_string_sc = to_string_i<shared_const>;
using to_string_co = to_string_i<const_observer>;

using to_string_u = to_string_i<unique>;
using to_string_mo = to_string_i<mutable_observer>;
}  // namespace

TEST_CASE("any lifetime cast") {
  const to_string_sc sc{std::make_shared<X>("hallo")};
  REQUIRE(sc.to_string() == "hallo");
  REQUIRE(is_derived_from<any_base<shared_const>>(sc));

  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void const*>>, void>);
  static_assert(std::same_as<std::decay_t<std::remove_pointer_t<void*>>, void>);
  // static_assert( std::same_as<std::decay_t<void const *>,
  // std::add_const_t<void*>);

  try {
    auto o1 = get_erased_data(sc);
    [[maybe_unused]] const auto x = unerase_cast<X>(sc);
    [[maybe_unused]] const auto x1 =
        static_cast<X const*>(get_void_data_ptr(sc));
    REQUIRE(x->s_ == "hallo");
  } catch (anyxx::type_mismatch_error&) {
    CHECK(false);
  }

  static_assert(std::same_as<to_string_co::v_table_t, to_string_sc::v_table_t>);
  static_assert(
      std::derived_from<to_string_sc::v_table_t, to_string_co::v_table_t>);
  to_string_co co{sc};  // NOLINT
  REQUIRE(co.to_string() == "hallo");
  static_assert(std::same_as<to_string_co::v_table_t, to_string_sc::v_table_t>);
  REQUIRE(is_derived_from<any_base<const_observer>>(co));

  to_string_u u{std::make_unique<X>("hallo")};  // NOLINT
  REQUIRE(u.to_string() == "hallo");
  static_assert(!is_typed_any<to_string_u>);
  static_assert(is_any<to_string_u>);
  static_assert(!constructibile_for<to_string_u, to_string_mo::erased_data_t>);
  static_assert(
      std::derived_from<to_string_mo::v_table_t, to_string_u::v_table_t>);
  to_string_mo mo{u};
  REQUIRE(mo.to_string() == "hallo");

  to_string_u u1{std::make_unique<X>("hallo")};
  REQUIRE(u1.to_string() == "hallo");
  to_string_co co_from_u{u1};
  REQUIRE(co_from_u.to_string() == "hallo");
}

namespace {
using to_string_sc_dynm = to_string_i<shared_const, dynm>;
using to_string_u_dynm = to_string_i<unique, dynm>;
using to_string_mo_dynm = to_string_i<mutable_observer, dynm>;
}  // namespace

TEST_CASE("any dynm lifetiem cast1") {
  auto sc1{to_string_u_dynm{std::make_unique<X>("hello dynm")}};
  CHECK(sc1.to_string() == "hello dynm");
}// NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
TEST_CASE("any dynm lifetiem cast2") {
  to_string_u_dynm u1_dynm{std::make_unique<X>("hello dynm")};
  CHECK(u1_dynm.to_string() == "hello dynm");
  static_assert(
      std::same_as<to_string_mo_dynm::v_table_t, to_string_u_dynm::v_table_t>);
  to_string_mo_dynm mo1{u1_dynm};
  CHECK(mo1.to_string() == "hello dynm");
}// NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)