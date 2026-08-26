#include <array>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <examples/anyxx26/dyn.hpp>
#include <examples/anyxx26/meta/print_members.hpp>
#include <examples/anyxx26/meta/utilities.hpp>
#include <examples/anyxx26/trait_as.hpp>
#include <meta>
#include <print>
#include <string>
#include <utility>
#include <vector>

using namespace anyxx26;

namespace {

template <typename Self, typename = anyxx26::declaration>
struct stringable {
  [[= defaulted]] static std::string as_string(Self const&);
};

void print(std::vector<dyn<stringable, anyxx::cref>> const& things) {
  for (auto& thing : things) {
    std::println("{}", thing.as_string());
  }
}

template <>
struct stringable<int, anyxx26::model_map> {
  static std::string as_string(int const& self) { return std::to_string(self); }
};

template <>
struct stringable<std::string, anyxx26::model_map> {
  static std::string as_string(std::string const& self) { return self; }
};

struct foo {
  double f;
};
template <>
struct stringable<foo, anyxx26::model_map> {
  static std::string as_string(foo const& self) {
    return "foo: " + std::to_string(self.f);
  }
};

struct boo {
  bool b = false;
  std::string as_string() const {
    return std::string{"boo? "} + (b ? "T" : "F");
  }
};
// Only to show how to delegate to the default adapter:
//template <>
//struct stringable<boo, anyxx26::model_map> {};

}  // namespace

TEST_CASE("anyxx26 hello world") {
  //auto a1 = trait_as<int, stringable>{42};
  //auto z_from_self = a1.as_string();
  //std::println("z_from_trait = {}", z_from_self);

  {
    using namespace anyxx;

    static_assert(
        std::is_same_v<base_v_table_t<stringable>, anyxx::observeable_v_table>);

    int i = 4711;
    std::string s = "hello world";
    foo a_foo{3.14};
    boo a_boo{true};
    print({i, s, a_foo, a_boo});
  }
  {
    dyn<stringable, anyxx::cref> sb{boo{true}};
    auto sb_str = sb.as_string();
    CHECK(sb_str == "boo? T");
  }
}

namespace {
template <typename Self, typename = anyxx26::declaration>
struct addable {
  [[= defaulted]] static void add(Self& self, int inc);
};

struct add1 {
  int value;
  void add(int x) { value += x; }
};
}  // namespace

TEST_CASE("anyxx26 mutable hello world") {
  using namespace anyxx;

  add1 a1{10};
  auto a1_dyn = dyn<addable, mutref>{a1};

  a1_dyn.add(5);
  CHECK(a1.value == 15);
  std::println("a1.value = {}", a1.value);
}

namespace {
template <typename Self, typename = anyxx26::declaration>
struct base_trait {
  [[= defaulted]] static std::string basef(Self const& self);
};

template <typename Self, typename = anyxx26::declaration>
 struct derived_trait : base_trait<Self> {
  [[= defaulted]] static std::string derivedf(Self const& self);
};

struct base_and_derived {
  std::string name;
  std::string basef() const { return "base " + name; }
  std::string derivedf() const { return "derived " + name; }
};

// template <>
// struct derived_trait<base_and_derived, anyxx26::model_map> {};

}  // namespace

TEST_CASE("anyxx26 derived trait") {
  using namespace anyxx;

  base_and_derived a1{"a1"};
  auto dyn1 = dyn<base_trait, cref>{a1};
  CHECK(dyn1.basef() == "base a1");
  //CHECK(dyn1.derivedf() == "derived");
  auto dyn2 = dyn<derived_trait, cref>{a1};
  CHECK(dyn2.basef() == "base a1");
  CHECK(dyn2.derivedf() == "derived a1");

  dyn<base_trait, cref> dyn3{dyn2};
  CHECK(dyn3.basef() == "base a1");
  base_and_derived a2{ "a2" };
  dyn<base_trait, cref> dyn4{a2};
  CHECK(dyn4.basef() == "base a2");
  dyn4 = dyn2;
  CHECK(dyn4.basef() == "base a1");

  dyn<base_trait, cref> dyn5{std::move(dyn2)};
  CHECK(dyn5.basef() == "base a1");
  dyn<base_trait, cref> dyn6{dyn1};
  CHECK(dyn6.basef() == "base a1");
  base_and_derived a3{ "a3" };
  dyn<derived_trait, cref> dyn7{ a3 };
  dyn6 = std::move(dyn7);
  CHECK(dyn6.basef() == "base a3");
}
