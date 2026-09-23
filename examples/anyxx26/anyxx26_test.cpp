#include <array>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/meta/print_members.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <bit_factory/v26/trait_as.hpp>
#include <meta>
#include <print>
#include <string>
#include <utility>
#include <vector>

using namespace anyxx26;

namespace {

template <is_trait Trait, typename Self>
struct stringable {
    std::string as_string() const;
};

void print(std::vector<any<stringable>> const& things) {
    for(auto& thing : things) {
        std::println("{}", thing.as_string());
    }
}

template <>
struct stringable<anyxx26::model_map, int> {
    static std::string as_string(int const& self) { return std::to_string(self); }
};

template <>
struct stringable<anyxx26::model_map, std::string> {
  static std::string as_string(std::string const& self) { return self; }
};

struct foo {
  double f;
};
template <>
struct stringable<anyxx26::model_map, foo> {
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

namespace{

template <typename DynBase>
consteval std::meta::info test_make_dyn_facade_call() {
    auto v_table_specs = get_interface_specs(^^stringable<declaration, declaration>);
    return make_dyn_facade_call<DynBase>(v_table_specs[0]);
}

}

TEST_CASE("anyxx26 make_dyn_facade_call") {
  using facade_call = [:test_make_dyn_facade_call<any_base<stringable, anyxx::cref>>():];
  struct dummy_dyn : any_base<stringable, anyxx::cref>, facade_call {
    using any_base<stringable, anyxx::cref>::any_base;
    using facade_call::operator();
  };

  int i = 4711;
  dummy_dyn x{i};
  auto s = x();
  CHECK(s == "4711");
}

TEST_CASE("anyxx26 hello world") {

  //dump_all<parent_of(^^stringable<void*>)>();
  //dump_type<stringable<void*>>();
  //dump_impl<stringable, int>();

  {
    int i = 4711;
    print({i});
    std::string s = "hello world";
    foo a_foo{3.14};
    boo a_boo{true};
    print({i, s, a_foo, a_boo});
  }
  {
    any<stringable> sb{boo{true}};
    auto sb_str = sb.as_string();
    CHECK(sb_str == "boo? T");
  }
}

namespace {
template <is_trait Trait, typename Self>
struct addable {
  [[= defaulted]] static void add(Self& self, int inc);
};

struct add1 {
  int value;
  void add(int x) { value += x; }
};
}  // namespace

TEST_CASE("anyxx26 mutable hello world") {
  //dump_type<addable<void*>>();
  //dump_impl<addable, add1>();

  add1 a1{10};
  auto a1_dyn = any<addable, anyxx::mutref>{a1};

  a1_dyn.add(5);
  CHECK(a1.value == 15);
  std::println("a1.value = {}", a1.value);
}

namespace {
template <is_trait, typename>
struct base_trait {
  std::string basef() const;
};

template <is_trait Trait, typename Self>
 struct derived_trait : base_trait<Trait, Self> {
  [[= defaulted]] static std::string derivedf(Self const& self);
};

struct base_and_derived {
  std::string name;
  std::string basef() const { return "base " + name; }
  std::string derivedf() const { return "derived " + name; }
};

}  // namespace

TEST_CASE("anyxx26 derived trait") {

  base_and_derived a1{"a1"};
  auto dyn1 = any<base_trait>{a1};
  CHECK(dyn1.basef() == "base a1");
  //CHECK(dyn1.derivedf() == "derived");
  auto dyn2 = any<derived_trait>{a1};
  CHECK(dyn2.basef() == "base a1");
  CHECK(dyn2.derivedf() == "derived a1");

  any<base_trait> dyn3{dyn2};
  CHECK(dyn3.basef() == "base a1");
  base_and_derived a2{ "a2" };
  any<base_trait> dyn4{a2};
  CHECK(dyn4.basef() == "base a2");
  dyn4 = dyn2;
  CHECK(dyn4.basef() == "base a1");

  any<base_trait> dyn5{std::move(dyn2)};
  CHECK(dyn5.basef() == "base a1");
  any<base_trait> dyn6{dyn1};
  CHECK(dyn6.basef() == "base a1");
  base_and_derived a3{ "a3" };
  any<derived_trait> dyn7{ a3 };
  dyn6 = std::move(dyn7);
  CHECK(dyn6.basef() == "base a3");
}

TEST_CASE("anyxx26 v_table_data") {

  base_and_derived a1{ "a1" };
  any<anyxx26::save_observable> d1{a1};
  //meta::print_members<any<anyxx26::save_observable>>();
  //meta::print_members<any<anyxx26::save_observable>::v_table_t::fptrs_t>();
  std::println("{}", d1.v_table_->type_info_->name());
  if(auto p = unerase_cast<base_and_derived>(d1)){
      CHECK(p->name == "a1");
  } else{
      CHECK(false);
  }
  [[maybe_unused]] constexpr auto member_type = ^^anyxx26::save_observable<declaration, declaration>::type_info_::type;
  static_assert(std::meta::is_type(member_type));
  static_assert(std::meta::is_type_alias(member_type));
  static_assert(std::meta::is_pointer_type(member_type));
  constexpr auto ctx = std::meta::access_context::current();
  constexpr static auto members1 = define_static_array(members_of(^^anyxx26::save_observable<declaration, declaration>, ctx));
  constexpr auto type_info_struct_meta = members1[0];
  using type = [:type_info_struct_meta:]::type;
  static_assert(std::meta::is_type(^^type));
  static_assert(std::meta::is_type_alias(^^type));
  static_assert(std::meta::is_pointer_type(^^type));
  static_assert(dealias(^^type) == ^^std::type_info const*);
  static_assert(std::same_as<type, anyxx26::save_observable<declaration, declaration>::type_info_::type>);
  //constexpr auto member_type1 = ^^[:type_info_struct_meta:]::type;
}

namespace {

template <is_trait Trait, typename Self, typename Value>
struct mapable {
    Value const& at(std::size_t) const;
    Value const& operator[](std::size_t) const;
    Value& at(std::size_t);
    Value& operator[](std::size_t);
    void set_all_to(Value const&);
};

template <typename Self, typename Value>
struct mapable<anyxx26::model_map, Self, Value> {
    static Value const& at(Self const& self, std::size_t i) {
        return self.at(i);
    }
    static Value& at(Self& self, std::size_t i) {
        return self.at(i);
    }
    static void set_all_to(Self& self, Value const& value) {
      for (auto& v : self) {
            v = value;
      }
    }
};

using any_base_test = any_base<mapable, anyxx::cref, int>;
using dyn_facade_test = [:make_dyn_facade<mapable, anyxx::cref, int>():];

static_assert(!has_deduced_typenames<stringable>);
static_assert(!has_deduced_typenames<addable>);
static_assert(std::same_as<deduced_typenames<addable>, empty_t>);
static_assert(!has_deduced_typenames<mapable, int>);

template <typename T>
concept has_set_all_to =requires(T t, int const& value) {
 { t.set_all_to(value) } -> std::same_as<void>;
};

}

TEST_CASE("anyxx26 templated trait") {

    //dump_type<mapable<void*, declaration, int>>();
    //dump_impl<mapable, std::vector<int>, int>();
    //anyxx26::meta::print_members<any<mapable, anyxx::cref, int>>();
    //anyxx26::meta::print_members<any<mapable, anyxx::cref, int>::v_table_t>();

    {
        std::vector<int> v1{1, 2};
        any<mapable, anyxx::cref, int> m{v1};
        decltype(auto) v = m.at(0);
        std::println("{}", v);
        static_assert(std::same_as<decltype(v), int const&>);
        CHECK(m.at(0) == 1);
        CHECK(m.at(1) == 2);
        CHECK(m[1] == 2);
        static_assert(!has_set_all_to<any<mapable, anyxx::cref, int>>);
    }
    {
        std::vector<int> v1{ 1, 2 };
        any<mapable, anyxx::mutref, int> m{ v1 };
        decltype(auto) v = m.at(0);
        std::println("{}", v);
        static_assert(std::same_as<decltype(v), int&>);
        CHECK(m.at(0) == 1);
        CHECK(m.at(1) == 2);
        CHECK(m[1] == 2);
        m.set_all_to(42);
        static_assert(has_set_all_to<any<mapable, anyxx::mutref, int>>);
    }
}

namespace {

template <is_trait Trait, typename Self>
struct operators : save_copyable<Trait, Self> {
    int const& operator*() const;
    Self operator+(int) const;
    Self& operator++();
    bool equal(Self const&) const;
    bool operator==(Self const&) const;
};

struct add_test {
    int i = 0;

    int const& operator*() const{
        return i;
    }
    add_test operator+(int inc) const{
        return { i + inc };
    }
    add_test& operator++() {
        ++i;
        return *this;
    }
    bool equal(add_test const& other) const {
        return i == other.i;
    }
    bool operator==(add_test const& other) const {
        return i == other.i;
    }
};

using int_ptr = int*;
template <>
struct operators<anyxx26::model_map, int_ptr> {
    static bool equal(int_ptr const& self, int_ptr const& other) {
        return self == other;
    }
};

}

TEST_CASE("anyxx26 operators") {
    {
        any<operators> ops{add_test{}};
        auto r1 = ops + 1;
        static_assert(^^decltype(r1) == ^^decltype(ops));
        CHECK(unerase_cast<add_test>(r1)->i == 1);
        //anyxx26::meta::print_members<any<operators>::v_table_t::fptrs_t>();
        anyxx26::meta::print_members<decltype(any<operators, anyxx::val<>>::op_plus_plus)>();
        auto& r2 = ++r1;
        CHECK(&unerase_cast<add_test>(r1)->i != &unerase_cast<add_test>(ops)->i);
        CHECK(&unerase_cast<add_test>(r1)->i == &unerase_cast<add_test>(r2)->i);
        CHECK(unerase_cast<add_test>(r1)->i == 2);
        auto r3 = r2++;
        CHECK(&unerase_cast<add_test>(r1)->i != &unerase_cast<add_test>(r3)->i);
        CHECK(unerase_cast<add_test>(r1)->i == 3);
        CHECK(unerase_cast<add_test>(r3)->i == 2);

        any<operators> ops_rhs{ add_test{3} };
	    CHECK(unerase_cast<add_test>(ops_rhs)->i == 3);
        CHECK(r1.equal(ops_rhs));
        CHECK(r1 == ops_rhs);
    }

    {
		std::array<int, 5> arr{ 1, 2, 3, 4, 5 };
		int* p_int = arr.begin();
        any<operators> p{p_int};
        CHECK(*p == 1);
		++p;
        CHECK(*p == 2);
        p = p + 2;
        CHECK(*p == 4);
    }
}

