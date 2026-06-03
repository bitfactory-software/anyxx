#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <variant>

namespace _2p_lib {

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
class A {
 public:
  int i = 0;
  explicit A(int i) : i(i) { assert(i >= 0); }

 private:
  A() = default;
  friend struct anyxx::using_<A>;
};
}  // namespace _2p_app
ANY_MODEL_MAP((_2p_app::A), _2p_lib::nullable) {
  using Rep = _2p_app::A;
  static bool has_value(_2p_app::A const& x) { return x.i != 0; };
  static _2p_app::A get_value(_2p_app::A const& x) { return x; };
};

TEST_CASE("_2p test optional 2") {
  using namespace anyxx;
  using namespace _2p_lib;
  using namespace _2p_app;

  optional<A> a1{A{42}};
  CHECK(a1.has_value());
  CHECK(a1.get_value().i == 42);

  optional<A> a2;
  CHECK(!a2.has_value());
}