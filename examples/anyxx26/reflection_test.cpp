#include <array>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <examples/anyxx26/dyn.hpp>
#include <examples/anyxx26/meta/utilities.hpp>
#include <examples/anyxx26/trait_as.hpp>
#include <meta>
#include <print>
#include <string>
#include <utility>
#include <vector>

static_assert(std::same_as<anyxx26::self_const_correct_t<int, const void*>, int const>);

//static_assert(std::is_const_v<typename[:remove_pointer(remove_reference(^^ const void*)):]>);
//static_assert(std::is_const_v<typename[:remove_pointer(remove_reference(^^ const void*&)):]>);
//static_assert(std::is_const_v<typename[:remove_pointer(remove_reference(^^ void const*)) :]>);
//static_assert(std::is_const_v<typename[:remove_pointer(remove_reference(^^ void* const&)) :]>);
//static_assert(std::is_const_v<typename[:(remove_reference(^^ const void*)) :]>);
//static_assert(std::is_const_v<typename[:(remove_reference(^^ const void*&)) :]>);
//static_assert(std::is_const_v<typename[:(remove_reference(^^ void const*)) :]>);
//static_assert(std::is_const_v<typename[:(remove_reference(^^ void* const&)) :]>);
//static_assert(std::same_as<typename[:remove_pointer(remove_reference(^^ const void*)) :], const void>);
//static_assert(std::same_as<typename[:remove_pointer(remove_reference(^^ const void*&)) :], const void>);
//static_assert(std::same_as<typename[:remove_pointer(remove_reference(^^ void const*)) :], const void>);
//static_assert(std::same_as<typename[:remove_pointer(remove_reference(^^ void* const&)) :], void const>);

//consteval{
//    if constexpr(std::is_const_v<typename[:remove_pointer(remove_reference(^^ const void*&)) :]>) {
//        static_assert(false);
//    } else { //if constexpr(std::same_as<typename[:type:], void*&>) {
//        static_assert(false);
//    }
//}

using namespace anyxx26;

namespace {

template <typename Self, bool base = true>
struct stringable {
  [[= defaulted]] static std::string as_string(Self const& self);
};

void print(std::vector<dyn<stringable, anyxx::mutref>> const& things) {
  for (auto& thing : things) {
    std::println("{}", thing.as_string());
  }
}

template <>
struct stringable<int> {
  static std::string as_string(int const& self) { return std::to_string(self); }
};

static_assert(std::same_as<typename[:remove_const(remove_reference(^^ int const&)) :], int>);
static_assert(std::is_const_v<typename[:remove_reference(^^ int const&) :]>);

template <>
struct stringable<std::string> {
  static std::string as_string(std::string const& self) { return self; }
};

struct foo {
  double f;
};
template <>
struct stringable<foo> {
  static std::string as_string(foo const& self) {
    return "foo: " + std::to_string(self.f);
  }
};

struct boo {
  bool b = false;
  std::string as_string() const { return std::string{"boo? "} + (b ? "T" : "F"); }
};
// Only to show how to delegate to the default adapter:
template <>
struct stringable<boo, false> : stringable<boo, true> {};

}  // namespace

TEST_CASE("anyxx26 hello world") {
  auto a1 = trait_as<int, stringable>{42};
  auto z_from_self = a1.as_string();
  std::println("z_from_trait = {}", z_from_self);

  {
    using namespace anyxx;

    static_assert(
        std::is_same_v<base_v_table_t<stringable>, anyxx::observeable_v_table>);

    int i = 4711;
    auto dyn_stringable = dyn<stringable, anyxx::mutref>{i};
    auto z_from_dyn_stringable = dyn_stringable.as_string();
    std::println("z_from_dyn_stringable = {}", z_from_dyn_stringable);

    std::string s = "hello world";
    foo a_foo{3.14};
    boo a_boo{true};
    print({i, s, a_foo, a_boo});
  }
}

namespace {
template <typename Self, bool base = true>
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
 template <typename Self, bool base = true>
 struct base_trait {
     [[= defaulted]] static std::string basef(Self const& self);
 };

 template <typename Self, bool base = true>
 struct derived_trait {//: base_trait<Self, base> {
     [[= defaulted]] static std::string derivedf(Self const& self);
 };

 struct base_and_derived {
   std::string basef() const { return "base"; }
   std::string derivedf() const{ return "derived"; }
 };
 }  // namespace

static_assert(std::is_const_v<std::remove_pointer_t<std::remove_reference_t<const void*>>>);
static_assert(std::same_as<anyxx26::self_const_correct_t<base_and_derived, const void*>, base_and_derived const>);

 TEST_CASE("anyxx26 derived trait") {
     using namespace anyxx;

     base_and_derived a1{};
     auto a1_dyn1 = dyn<base_trait, cref>{ a1 };
     CHECK(a1_dyn1.basef() == "base");
     auto a_dyn2 = dyn<derived_trait, cref>{ a1 };
     //CHECK(a_dyn2.basef() == "base");
     CHECK(a_dyn2.derivedf() == "derived");
}


