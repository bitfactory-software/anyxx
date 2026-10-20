#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>

using namespace anyxx;

namespace {

TRAIT(stringable, observeable,
      (FN_IMPL(public, std::string, as_string, (), const,
               () { return std::format("{}", self); })))

MODEL_MAP(bool, stringable) {
  std::string as_string(bool const& self) { return self ? "wahr" : "falsch"; };
};

MODEL_MAP(double, stringable) {
  std::string as_string(const double& self) {
    return std::format("{:6.3}", self);
  };
};

template <typename V>
std::string print(trait_as<V, stringable> const& s) {
  static_assert(sizeof(trait_as<V, stringable>) == sizeof(V));
  return s.as_string() + "\n";
}
template <typename V>
auto print(V s) {
  return print(trait_as<V, stringable>(std::move(s)));
}

}  // namespace

TEST_CASE("talk_23_6_trait_as") {
  CHECK(print("Hello world!") == "Hello world!\n");
  CHECK(print(true) == "wahr\n");
  CHECK(print(3.14) == "  3.14\n");
  CHECK(print(42) == "42\n");
}
