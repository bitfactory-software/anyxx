#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace anyxx;

static_assert(anyxx::is_in_dll_mode == false);

namespace {
struct X {
  static inline int tracker_ = 0;
  X(std::string const& s = "") : s_(s) { ++tracker_; }
  ~X() { --tracker_; }
  X(X const& x) : s_(x.s_) { ++tracker_; }
  X(X&& x) : s_(std::move(x.s_)) {}
  X& operator=(X const& x) { s_ = x.s_; return *this; }
  X& operator=(X&& x) { s_ = std::move(x.s_); return *this; }
  std::string s_;
  [[nodiscard]] std::string to_string() const { return s_; }
};

ANY(any_stringable, (ANY_METHOD(std::string, to_string, (), const)), , )

}  // namespace

TEST_CASE("any lifetime cast") {
  const any_stringable<shared_const> sc{std::make_shared<X>("hallo")};
  REQUIRE(sc.to_string() == "hallo");
  REQUIRE(is_derived_from<any<shared_const>>(sc));

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

  static_assert(std::same_as<any_stringable<const_observer>::v_table_t, any_stringable<shared_const>::v_table_t>);
  static_assert(
      std::derived_from<any_stringable<shared_const>::v_table_t, any_stringable<const_observer>::v_table_t>);
  any_stringable<const_observer> co{sc};  // NOLINT
  REQUIRE(co.to_string() == "hallo");
  static_assert(std::same_as<any_stringable<const_observer>::v_table_t, any_stringable<shared_const>::v_table_t>);
  REQUIRE(is_derived_from<any<const_observer>>(co));

  any_stringable<unique> u{std::make_unique<X>("hallo")};  // NOLINT
  REQUIRE(u.to_string() == "hallo");
  static_assert(!is_typed_any<any_stringable<unique>>);
  static_assert(is_any<any_stringable<unique>>);
  static_assert(!constructibile_for<any_stringable<unique>, any_stringable<mutable_observer>::erased_data_t>);
  static_assert(
      std::derived_from<any_stringable<mutable_observer>::v_table_t, any_stringable<unique>::v_table_t>);
  any_stringable<mutable_observer> mo{u};
  REQUIRE(mo.to_string() == "hallo");

  any_stringable<unique> u1{std::make_unique<X>("hallo")};
  REQUIRE(u1.to_string() == "hallo");
  any_stringable<const_observer> co_from_u{u1};
  REQUIRE(co_from_u.to_string() == "hallo");
}
