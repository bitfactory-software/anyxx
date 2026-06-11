#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <variant>

namespace _2p_lib {

TRAIT(equal_comparable,
      (ANY_OP_MAP_NAMED_FRIEND(bool, ==, eq, (anyxx::self const&), const),
       ANY_OP_DEF(public, bool, !=, ne, (anyxx::self const&), const,
                  [&x](auto const& r) {
                    return !(trait_as<equal_comparable>(x) ==
                             trait_as<equal_comparable>(r));
                  })))

// An optional as a customizable trait.
// It is default represented as a variant of the value type and a
// monostate. When there is a class template called 'trait-name'_default_rep,
// this type is used as the representation;
// This type must be constructible from the type 'T' being modeled.
// If 'trait-name'_default_rep is default constructible, then the any using this
// trait  is default constructible.
// In our case is this effect used to models the 'no value' state.
// 'has_value' and 'get_value' are implemented in terms of the variant.
template <typename T>
struct nullable_default_rep : std::variant<std::monostate, T> {
  using variant_t = std::variant<std::monostate, T>;
  using variant_t::variant;
  friend bool operator==(nullable_default_rep const& x,nullable_default_rep const& r) = default;
};

TRAIT(nullable,
      (ANY_FN_DEF(public, bool, has_value, (), const,
                  [x]() { return std::holds_alternative<T>(x); }),
       ANY_FN_DEF_EXACT(public, T const&, get_value, (), const,
                        [&x]() -> T const& { return std::get<T>(x); }),
       ANY_FN_DEF_EXACT(public, T&, get_value, (), ,
                        [&x]() -> T& { return std::get<T>(x); }),
       ANY_OP_DEF_EXACT(public, T const&, ->, arrow_const, (), const,
                        [&x]() -> T const& { return Map{}.get_value(x); }),
       ANY_OP_DEF_EXACT(public, T&, ->, arrow, (), ,
                        [&x]() -> T& { return Map{}.get_value(x); }),
       ANY_OP_DEF_EXACT(public, T const&, *, deref_const, (), const,
                        [&x]() -> T const& { return Map{}.get_value(x); }),
       ANY_OP_DEF_EXACT(public, T&, *, deref, (), ,
                        [&x]() -> T& { return Map{}.get_value(x); }),
       ANY_OP_DEF(public, bool, ==, eq, (anyxx::self const&), const,
                  [&x](auto const& r) { return x == r; })))

template <typename T>
using optional = anyxx::using_<T>::template as<nullable>;
}  // namespace _2p_lib

TEST_CASE("_2p test optional 1") {
  using namespace anyxx;
  using namespace _2p_lib;

  optional<int> i1{42};
  CHECK(i1.has_value());
  CHECK(i1.get_value() == 42);
  CHECK(*i1 == 42);
  CHECK((i1 == optional<int>{42}));
  CHECK((i1 != optional<int>{43}));
  CHECK((i1 != optional<int>{}));

  optional<int> i2;
  CHECK(!i2.has_value());
  CHECK((i2 == optional<int>{}));
}

namespace _2p_app {
class foo {
 public:
  class voo {
   public:
    int i = 0;
  };
  decltype(auto) operator->(this auto&& self) { return self.v_.get(); }
  explicit foo(int i) : v_(std::make_shared<voo>(i)) {}
  friend bool operator==(foo const& self, foo const& r) = default;

 private:
  foo() = default;
  std::shared_ptr<voo> v_;
  friend struct anyxx::using_<foo>;
  friend struct _2p_lib::nullable_model_map<foo>;
};
}  // namespace _2p_app
// class foo has an internal state for 'no value'(v_ == nullptr), so we can
// directly use it as the representation of the nullable trait. This is done be
// seting 'rep_type' to 'foo' and providing the 'has_value' and 'get_value'
// functions accordingly.
ANY_MODEL_MAP((_2p_app::foo), _2p_lib::nullable) {
  using rep_type = _2p_app::foo;
  bool has_value(_2p_app::foo const& x) { return x.v_ != nullptr; };
  auto& get_value(auto&& x) { return std::forward<decltype(x)>(x); };
};

namespace {
template <typename T>
bool fun(anyxx::any<anyxx::using_<T>, _2p_lib::equal_comparable> const& l,
         anyxx::any<anyxx::using_<T>, _2p_lib::equal_comparable> const& r) {
  return r == l;
}
static_assert(
    _2p_lib::is_equal_comparable_model<
        anyxx::any<anyxx::using_<_2p_app::foo>, _2p_lib::equal_comparable>>);
}  // namespace

TEST_CASE("_2p test optional 2") {
  using namespace anyxx;
  using namespace _2p_lib;
  using namespace _2p_app;

  optional<foo> a1{foo{42}};
  //+++ the point of this example: no overhead for 'empty' indicator!
  static_assert(sizeof(a1) == sizeof(foo));
  //---
  CHECK(a1.has_value());
  CHECK(a1.get_value()->i == 42);
  CHECK(a1->i == 42);
  CHECK((*a1)->i == 42);
  auto a1x = a1;
  CHECK(a1 == a1x);
  CHECK(fun<optional<foo>>(a1, a1x));
  CHECK(a1 != optional<foo>{42});
  CHECK(a1 != optional<foo>{43});
  CHECK(a1 != optional<foo>{});

  optional<foo> a2;
  CHECK(!a2.has_value());
  CHECK(a2 == optional<foo>{});
}