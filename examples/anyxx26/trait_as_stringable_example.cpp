#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/trait_as.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>

using namespace anyxx26;

namespace {

template <is_trait Trait, typename Self>
struct stringable {
    static std::string as_string(Self const& s) { 
        return std::format("{}", s);
    }
};

template <>
struct stringable<model_map, bool> {
  static std::string as_string(bool const& self) { return self ? "wahr" : "falsch"; }
};

template <>
struct stringable<model_map, double> {
  static std::string as_string(const double& value) {
    return std::format("{:6.3}", value);
  }
};

template <typename V>
std::string print_(trait_as<V, stringable> const& s) {
  return s.as_string() + "\n";
}
template <typename V>
auto print(V s)
//  requires stringable_trait<V>::is_defined
{
  return print_(trait_as<V, stringable>(std::move(s)));
}

template <class V>
concept is_print_callable = requires(V v) {
  { print(v) } -> std::same_as<std::string>;
};

}

TEST_CASE("example trait_as stringable") {
  CHECK(print("Hello world!") == "Hello world!\n");
  CHECK(print(true) == "wahr\n");
  CHECK(print(3.14) == "  3.14\n");
  CHECK(print(42) == "42\n");
}
