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
    std::string as_string() const;
};

void print(std::vector<dyn<stringable, anyxx::cref>> const& things) {
    for(auto& thing : things) {
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
template <std::meta::info ns>
void dump_all(){
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(ns, ctx))) {
        if constexpr(has_identifier(m)) {
            std::println("{}", std::meta::display_string_of(m));
        }
    }
}

template <typename T>
void dump_type(){
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(^^T, ctx))) {
        if constexpr(has_identifier(m) && is_function(m)) {
            constexpr auto ft = make_v_table_fptr_type<m>();
            using ft_t = typename [:ft:];
            std::println("{} -> {}", std::meta::display_string_of(m), std::meta::display_string_of(ft));
            std::println("   {}", typeid(ft_t).name());
        }
    }
}

template <template <typename, typename...> typename Trait, typename Concrete, typename... Args>
void dump_impl(){
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(^^Trait<void*, Args..., anyxx26::declaration>, ctx))) {
        if constexpr(has_identifier(m) && is_function(m)) {
            constexpr auto fi = find_function_impl<^^Trait, Concrete, m, Args...>();
            std::println("{} -> {}", std::meta::display_string_of(m), std::meta::display_string_of(fi));
            constexpr auto vfimpl = make_vfimpl<Concrete, fi>();
            std::println("{} -> {}", std::meta::display_string_of(m), std::meta::display_string_of(vfimpl));
            std::println("   {}", typeid( [:vfimpl:]).name());
        }
    }
}

}  // namespace

TEST_CASE("anyxx26 hello world") {

  dump_all<parent_of(^^stringable<void*>)>();
  dump_type<stringable<void*>>();
  dump_impl<stringable, int>();

  {
    int i = 4711;
    print({i});
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

  dump_type<addable<void*>>();
  dump_impl<addable, add1>();

  add1 a1{10};
  auto a1_dyn = dyn<addable, mutref>{a1};

  a1_dyn.add(5);
  CHECK(a1.value == 15);
  std::println("a1.value = {}", a1.value);
}

namespace {
template <typename Self, typename = anyxx26::declaration>
struct base_trait {
  std::string basef() const;
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

TEST_CASE("anyxx26 v_table_data") {
  using namespace anyxx;

  base_and_derived a1{ "a1" };
  dyn<anyxx26::save_observable, cref> d1{a1};
  meta::print_members<dyn<anyxx26::save_observable, cref>>();
  meta::print_members<dyn<anyxx26::save_observable, cref>::v_table_t::fptrs_t>();
  std::println("{}", d1.v_table_->type_info_->name());
  if(auto p = unerase_cast<base_and_derived>(d1)){
      CHECK(p->name == "a1");
  } else{
      CHECK(false);
  }
  [[maybe_unused]] constexpr auto member_type = ^^anyxx26::save_observable<void*>::type_info_::type;
  static_assert(std::meta::is_type(member_type));
  static_assert(std::meta::is_type_alias(member_type));
  static_assert(std::meta::is_pointer_type(member_type));
  constexpr auto ctx = std::meta::access_context::current();
  constexpr static auto members1 = define_static_array(members_of(^^anyxx26::save_observable<void*>, ctx));
  constexpr auto type_info_struct_meta = members1[0];
  using type = [:type_info_struct_meta:]::type;
  static_assert(std::meta::is_type(^^type));
  static_assert(std::meta::is_type_alias(^^type));
  static_assert(std::meta::is_pointer_type(^^type));
  static_assert(dealias(^^type) == ^^std::type_info const*);
  static_assert(std::same_as<type, anyxx26::save_observable<void*>::type_info_::type>);
  //constexpr auto member_type1 = ^^[:type_info_struct_meta:]::type;
}

namespace {

template <typename Self, typename, typename Value>
struct mapable {
    Value const& at(std::size_t) const;
};

template <typename Self, typename Value>
struct mapable<Self, anyxx26::model_map, Value> {
    static Value const& at(Self const& self, std::size_t i) {
        return self.at(i);
    }
};

using dyn_base_test = dyn_base<mapable, anyxx::cref, int>;
using dyn_facade_test = [:make_dyn_facade<mapable, anyxx::cref, int>():];

}

TEST_CASE("anyxx26 templated trait") {

    dump_type<mapable<void*, declaration, int>>();
    dump_impl<mapable, std::vector<int>, int>();

    {
        std::vector<int> v1{1, 2};
        dyn<mapable, anyxx::cref, int> m{v1};
        auto v = m.at(0);
        std::println("{}", v);
        CHECK(m.at(0) == 1);
        CHECK(m.at(1) == 2);
    }
}

namespace { 
using any_copyable = dyn<save_copyable, anyxx::val<>>;
using any_moveable = dyn<save_moveable, anyxx::val<>>;

}  // namespace

TEST_CASE("anyxx26 std type erasure equivalents") {
    {
        auto a1 = any_copyable{ 42 };
        if(auto p = unerase_cast<int>(a1)){
            CHECK(*p == 42);
        } else{
            CHECK(false);
        }
        if(unerase_cast<double>(a1)){
            CHECK(false);
        } else{
            CHECK(true);
        }
        any_copyable a2 = a1;
        if(auto p = unerase_cast<int>(a2)){
            CHECK(*p == 42);
        } else{
            CHECK(false);
        }
        a1 = std::string{ "hello" };
        if(auto p = unerase_cast<std::string>(a1)){
            CHECK(*p == "hello");
        } else {
            CHECK(false);
        }
        if(auto p = unerase_cast<int>(a2)){
            CHECK(*p == 42);
        } else{
            CHECK(false);
        }
    }
    {
        auto a1 = any_moveable{ std::make_unique<int>(42) };
        if(auto p = unerase_cast<std::unique_ptr<int>>(a1)){
            CHECK(*p->get() == 42);
        } else{
            CHECK(false);
        }
        any_moveable a2 = std::move(a1);
        if(auto p = unerase_cast<std::unique_ptr<int>>(a2)){
            CHECK(*p->get() == 42);
        } else{
            CHECK(false);
        }
		static_assert(!std::copy_constructible<any_moveable>);
		//any_moveable a3 = a2; does not compile, as expected
    }
}
