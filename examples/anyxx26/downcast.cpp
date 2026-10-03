#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/anys/function.hpp>
#include <bit_factory/v26/meta/print_members.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <bit_factory/v26/trait_as.hpp>
#include <catch2/catch_test_macros.hpp>
#include <meta>
#include <print>
#include <string>
#include <utility>
#include <vector>

using namespace anyxx26;

namespace {

template <is_trait Trait, typename Self>
struct base_trait : dynamic_castable<Trait, Self> {
  std::string as_string() const;
};

template <is_trait Trait, typename Self>
struct derived_trait : base_trait<Trait, Self> {
  std::string decorated() const;
};

template <>
struct base_trait<anyxx26::model_map, int> {
  static std::string as_string(int const& self) { return std::to_string(self); }
};

template <>
struct base_trait<anyxx26::model_map, std::string> {
  static std::string as_string(std::string const& self) { return self; }
};

template <>
struct derived_trait<anyxx26::model_map, int> {
  static std::string decorated(int const& self) {
    return "int: " + std::to_string(self);
  }
};

template <>
struct derived_trait<anyxx26::model_map, std::string> {
  static std::string decorated(std::string const& self) {
    return "string: " + self;
  }
};

std::string delegate(
    std::vector<any<base_trait>> const& things,
    any<function, std::string(any<base_trait> const&)> process) {
  std::string result;
  for (auto const& thing : things) {
    result += process(thing) + "\n";
  }
  return result;
}

}  // namespace

TEST_CASE("anyxx26 downcast") {

  int i = 4711;
  std::string s = "hello world";

  any<derived_trait> d1{i};
  any<base_trait> b1{d1};
  CHECK((void*)d1.v_table_ == (void*)b1.v_table_);
  auto derived_v_table1 =
      unchecked_v_table_downcast_to<any<derived_trait>::v_table_t>(b1.v_table_);
  CHECK((void*)derived_v_table1 == (void*)d1.v_table_);
  static_assert(anyxx::is_any_derived_from_v<any<derived_trait>::v_table_t,
                                             any<base_trait>::v_table_t>);

  auto derived_v_table2 =
      unchecked_v_table_downcast_to<any<derived_trait>>(b1.v_table_);
  CHECK((void*)derived_v_table2 == (void*)d1.v_table_);

  auto dd = anyxx::downcast_to<any<derived_trait>>(b1);
  static_assert(std::same_as<decltype(dd), std::optional<any<derived_trait>>>);
  CHECK(dd->decorated() == "int: 4711");

  auto processed =
      delegate({any<derived_trait>{i}, any<derived_trait>{s}}, [](any<base_trait> const& thing) {
        auto derived = anyxx::downcast_to<any<derived_trait>>(thing);
        CHECK(derived.has_value());
        return derived->decorated();
      });
  CHECK(processed == "int: 4711\nstring: hello world\n");
}
