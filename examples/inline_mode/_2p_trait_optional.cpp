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

TEST_CASE("test optional") {
  using namespace anyxx;
  using namespace _2p_lib;

  optional<int> i1{42};
  CHECK(i1.has_value());
  CHECK(i1.get_value() == 42);

  optional<int> i2;
  CHECK(!i2.has_value());
}