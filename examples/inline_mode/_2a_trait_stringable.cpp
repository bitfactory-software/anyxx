#include <algorithm>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <string>

namespace example_2a {

ANY(stringable, (ANY_METHOD_DEFAULTED(std::string, to_string, (), const,
                                      [&x]() { return std::format("{}", x); })), )

}

ANY_MODEL_MAP((bool), example_2a::any_stringable) {
  static std::string to_string(bool const& value) {
    return value ? "wahr" : "falsch";
  };
};

ANY_MODEL_MAP((double), example_2a::any_stringable) {
  static std::string to_string(const double& value) {
    return std::format("{:6.3}", value);
  };
};

namespace example_2a {

template <typename V>
std::string print_(any_stringable_trait<V> const& s) {
  return s.to_string() + "\n";
}
template <typename V>
auto print(V s)
//  requires stringable_trait<V>::is_defined
{
  return print_(anyxx::trait_as<any_stringable>(std::move(s)));
}

}  // namespace example_2a

template <class V>
concept is_print_callable = requires(V v) {
  { print(v) } -> std::same_as<std::string>;
};

TEST_CASE("example 2a stringable") {
  using namespace example_2a;
  CHECK(print(true) == "wahr\n");
  CHECK(print(3.14) == "  3.14\n");
  CHECK(print(42) == "42\n");
  //  static_assert(!is_print_callable<int>);
  // print(42);  // remove comment to see the compilation error!
}
