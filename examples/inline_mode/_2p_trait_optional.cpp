#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <variant>

namespace _2p_lib {

// An optional as a customizable trait.
// It is default represented as a variant of the value type and a
// monostate. When there is a class template called 'trait-name'_default_rep,
// this type is used as the representation;
// This type must be constructible from the type 'T' being modeled.
// If 'trait-name'_default_rep is default constructible, then the any using this
// trait is default constructible.
// In this example this models the 'no value' state.
// 'has_value' and 'get_value' are implemented in terms of the variant.
template <typename T>
struct nullable_default_rep : std::variant<std::monostate, T> {
  using std::variant<std::monostate, T>::variant;
};
TRAIT(nullable, (ANY_FN_DEF(public, bool, has_value, (), const,
                            [x]() { return std::holds_alternative<T>(x); }),
                 ANY_FN_DEF(public, T, get_value, (), const,
                            [x]() { return std::get<T>(x); })))

template <typename T>
using optional = anyxx::using_<T>::template as<nullable>;
}  // namespace _2p_lib

TEST_CASE("_2p test optional 1") {
  using namespace anyxx;
  using namespace _2p_lib;

  optional<int> i1{42};
  CHECK(i1.has_value());
  CHECK(i1.get_value() == 42);

  optional<int> i2;
  CHECK(!i2.has_value());
}

namespace _2p_app {
class foo {
 public:
  class voo {
   public:
    int i = 0;
  };
  voo* operator->() { return v_.get(); }
  int i = 0;
  explicit foo(int i) : v_(std::make_shared<voo>(i)) {}

 private:
  foo() = default;
  std::shared_ptr<voo> v_;
  friend struct anyxx::using_<foo>;
  friend struct _2p_lib::nullable_model_map<foo>;
};
}  // namespace _2p_app
// class foo has an internal state for 'no value'(v_ == nullptr), so we can
// directly use it as the representation of the nullable trait. This is done be
// seting rep_type to 'foo' and providing the has_value and get_value functions
// accordingly. We use this indriection
ANY_MODEL_MAP((_2p_app::foo), _2p_lib::nullable) {
  using rep_type = _2p_app::foo;
  static bool has_value(_2p_app::foo const& x) { return x.v_ != nullptr; };
  static _2p_app::foo get_value(_2p_app::foo const& x) { return x; };
};

TEST_CASE("_2p test optional 2") {
  using namespace anyxx;
  using namespace _2p_lib;
  using namespace _2p_app;

  optional<foo> a1{foo{42}};
  CHECK(a1.has_value());
  CHECK(a1.get_value()->i == 42);

  optional<foo> a2;
  CHECK(!a2.has_value());
}