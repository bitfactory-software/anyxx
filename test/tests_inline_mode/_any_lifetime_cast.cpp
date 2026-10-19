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
  X& operator=(X&& x) noexcept {
    s_ = std::move(x.s_);
    return *this;
  }
  std::string s_;
  [[nodiscard]] std::string operator()() const { return s_; }
};

TRAIT(stringable, dynamic_deletable, (OP_(std::string, (), (), const)))
TRAIT(slick_stringable, observeable, (OP_(std::string, (), (), const)))

struct Y {
  any_<mutref, slick_stringable> xxx;
};

std::string func(any_<cref, slick_stringable> x) { return x(); }

}  // namespace

TEST_CASE("slick_stringable observeable_v_table") {
  {
    X f("hallo");
    any<slick_stringable> a{f};
    // bool derived = is_derived_from(typeid(slick_stringable), a);
    // CHECK(!derived);
    CHECK(a() == f());

    any<slick_stringable> b = a;
    any<slick_stringable> c = std::move(b);
    any<slick_stringable> d{a};
    any<slick_stringable> e{std::move(d)};
    auto eq = a() == func(e);
    a = e;
    CHECK(eq);
    auto x = unchecked_any_cast<X>(a);
    CHECK((*x)() == f());
    CHECK(X::tracker_ == 1);
  }
  {
    X f("hallo");
    const any_<mutref, slick_stringable> a{f};
    // bool derived = is_derived_from(typeid(slick_stringable), a);
    // CHECK(!derived);
    CHECK(a() == f());
  }
  {
    X f("hallo");
    Y y{.xxx = any_<mutref, slick_stringable>{f}};
  }
}

TEST_CASE("any_ lifetime cast") {
  const any_<shared, stringable> sc{std::make_shared<X>("hallo")};
  REQUIRE(sc() == "hallo");
  REQUIRE(is_derived_from<any<dynamic_deletable>>(sc));

  static_assert(
      std::same_as<std::decay_t<std::remove_pointer_t<void const*>>, void>);
  static_assert(std::same_as<std::decay_t<std::remove_pointer_t<void*>>, void>);
  // static_assert( std::same_as<std::decay_t<void const *>,
  // std::add_const_t<void*>);

  try {
    auto o1 = get_proxy(sc);
    [[maybe_unused]] decltype(auto) x = any_cast<X>(sc);
    [[maybe_unused]] const auto x1 = static_cast<X const*>(get_proxy_ptr(sc));
    REQUIRE(x.s_ == "hallo");
  } catch (anyxx::type_mismatch_error&) {
    CHECK(false);
  }

  static_assert(std::same_as<any_<cref, stringable>::v_table_t,
                             any_<shared, stringable>::v_table_t>);
  static_assert(std::derived_from<any_<shared, stringable>::v_table_t,
                                  any_<cref, stringable>::v_table_t>);
  any_<cref, stringable> co{sc};  // NOLINT
  REQUIRE(co() == "hallo");
  static_assert(std::same_as<any_<cref, stringable>::v_table_t,
                             any_<shared, stringable>::v_table_t>);
  REQUIRE(is_derived_from<any_<cref, dynamic_deletable>>(co));

  any_<unique, stringable> u{std::make_unique<X>("hallo")};  // NOLINT
  REQUIRE(u() == "hallo");
  static_assert(!is_typed_any<any_<unique, stringable>>);
  static_assert(is_any<any_<unique, stringable>>);
  static_assert(
      !constructibile_for<any_<unique, stringable>,
                          any_<mutref, stringable>::proxy_t, stringable<>>);
  static_assert(std::derived_from<any_<mutref, stringable>::v_table_t,
                                  any_<unique, stringable>::v_table_t>);
  any_<mutref, stringable> mo{u};
  REQUIRE(mo() == "hallo");

  any_<unique, stringable> u1{std::make_unique<X>("hallo")};
  REQUIRE(u1() == "hallo");
  any_<cref, stringable> co_from_u{u1};
  REQUIRE(co_from_u() == "hallo");
}
