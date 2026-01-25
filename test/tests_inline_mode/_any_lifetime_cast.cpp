#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace anyxx;

static_assert(anyxx::is_in_dll_mode == false);

namespace {
struct X {
  static inline int tracker_ = 0;
  X(std::string s = "") : s_(std::move(s)) { ++tracker_; }
  ~X() { --tracker_; }
  X(X const& x) : s_(x.s_) { ++tracker_; }
  X(X&& x) noexcept : s_(std::move(x.s_)) {}
  X& operator=(X const& x) = default;
  X& operator=(X&& x) noexcept { s_ = std::move(x.s_); return *this; }
  std::string s_;
  [[nodiscard]] std::string operator()() const { return s_; }
};

ANY(stringable, (ANY_OP(std::string, (), (), const)), )

}  // namespace

TEST_CASE("any lifetime cast") {
  const any_stringable<shared> sc{std::make_shared<X>("hallo")};
  REQUIRE(sc() == "hallo");
  REQUIRE(is_derived_from<any<shared>>(sc));

  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void const*>>, void>);
  static_assert(std::same_as<std::decay_t<std::remove_pointer_t<void*>>, void>);
  // static_assert( std::same_as<std::decay_t<void const *>,
  // std::add_const_t<void*>);

  try {
    auto o1 = get_proxy(sc);
    [[maybe_unused]] const auto x = unerase_cast<X>(sc);
    [[maybe_unused]] const auto x1 =
        static_cast<X const*>(get_proxy_ptr(sc));
    REQUIRE(x->s_ == "hallo");
  } catch (anyxx::type_mismatch_error&) {
    CHECK(false);
  }

  static_assert(std::same_as<any_stringable<cref>::v_table_t, any_stringable<shared>::v_table_t>);
  static_assert(
      std::derived_from<any_stringable<shared>::v_table_t, any_stringable<cref>::v_table_t>);
  any_stringable<cref> co{sc};  // NOLINT
  REQUIRE(co() == "hallo");
  static_assert(std::same_as<any_stringable<cref>::v_table_t, any_stringable<shared>::v_table_t>);
  REQUIRE(is_derived_from<any<cref>>(co));

  any_stringable<unique> u{std::make_unique<X>("hallo")};  // NOLINT
  REQUIRE(u() == "hallo");
  static_assert(!is_typed_any<any_stringable<unique>>);
  static_assert(is_any<any_stringable<unique>>);
  static_assert(!constructibile_for<any_stringable<unique>, any_stringable<mutref>::proxy_t>);
  static_assert(
      std::derived_from<any_stringable<mutref>::v_table_t, any_stringable<unique>::v_table_t>);
  any_stringable<mutref> mo{u};
  REQUIRE(mo() == "hallo");

  any_stringable<unique> u1{std::make_unique<X>("hallo")};
  REQUIRE(u1() == "hallo");
  any_stringable<cref> co_from_u{u1};
  REQUIRE(co_from_u() == "hallo");
}
