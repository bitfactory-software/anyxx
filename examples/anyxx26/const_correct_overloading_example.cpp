#include <array>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <bit_factory/v26/any.hpp>
#include <bit_factory/v26/trait_as.hpp>
#include <print>

using namespace anyxx26;

namespace {

template <is_trait Trait, typename Self, typename Value>
struct mapable {
    Value const& at(std::size_t) const;
    Value const& operator[](std::size_t) const;
    Value& at(std::size_t);
    Value& operator[](std::size_t);
    std::size_t size() const;
    void set_all_to(Value const&);
};

template <typename Self, typename Value>
struct mapable<anyxx26::model_map, Self, Value> {
    //static Value const& at(Self const& self, std::size_t i) {
    //    return self.at(i);
    //}
    //static Value& at(Self& self, std::size_t i) {
    //    return self.at(i);
    //}
    static void set_all_to(Self& self, Value const& value) {
      for (auto& v : self) {
            v = value;
      }
    }
};

template <typename T>
concept has_set_all_to =requires(T t, int const& value) {
 { t.set_all_to(value) } -> std::same_as<void>;
};

}

TEST_CASE("anyxx26 const correctness overloading any") {
    {
        std::vector<int> v1{1, 2};
        any<mapable, anyxx::cref, int> m{v1};
        decltype(auto) v = m.at(0);
        std::println("{}", v);
        static_assert(std::same_as<decltype(v), int const&>);
        CHECK(m.at(0) == 1);
        CHECK(m.at(1) == 2);
        CHECK(m[1] == 2);
        CHECK(m.size() == 2);
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
        CHECK(m.size() == 2);
        static_assert(has_set_all_to<any<mapable, anyxx::mutref, int>>);
    }
}

TEST_CASE("anyxx26 const correctness overloading trait_as") {
    consteval{
        //constexpr auto trait_declaration = anyxx26::trait_declaration_t<mapable, int>;
        //constexpr auto trait_model_map = anyxx26::trait_model_map<mapable, std::vector<int>>;
        constexpr auto spec = ^^mapable<declaration, std::vector<int>, int>::size;
        constexpr auto target = find_candidate_in_target<spec, std::vector<int>, void, void const*>();
        static_assert(target == ^^std::vector<int>::size);
        constexpr auto target2 = find_candidate_in<std::vector<int>, spec>();
        static_assert(target2 == ^^std::vector<int>::size);
        
        constexpr auto interface_specs_with_target = std::define_static_array(make_interface_specs_with_target<std::vector<int>, mapable, int>());
        static_assert(interface_specs_with_target.size() == 6);
        constexpr auto s1 = display_string_of(interface_specs_with_target[0].target);
        constexpr auto s2 = display_string_of(interface_specs_with_target[1].target);
        static_assert(s1 != s2);
        constexpr auto s3 = std::define_static_string(display_string_of(interface_specs_with_target[3].target));
        constexpr auto s4 = std::define_static_string(display_string_of(interface_specs_with_target[3].member));
        static_assert(s3 != s4);

        //constexpr auto return_type = translate_trait_as_return_type<trait_as<std::vector<int>, mapable, int>>(interface_specs_with_target[3].member);
        //constexpr auto return_type_str = std::define_static_string(display_string_of(return_type));
        //static_assert(std::same_as<[:return_type:], int&>);
        //throw std::meta::exception("return_type: " + std::string{ display_string_of(return_type) }, return_type);
        //auto params = make_trait_as_params<trait_as<std::vector<int>, mapable, int>>(interface_specs_with_target[3].member);
        //throw std::meta::exception("params: " + std::string{ display_string_of(params[0]) } , interface_specs_with_target[3].member);
    }

    {
        std::vector<int> v1{ 1, 2 };
        auto const m = using_<std::vector<int>>::as<mapable, int>(v1);
        decltype(auto) v = m.at(0);
        std::println("{}", v);
        static_assert(std::same_as<decltype(v), int const&>);
        CHECK(m.at(0) == 1);
        CHECK(m.at(1) == 2);
        CHECK(m[1] == 2);
        CHECK(m.size() == 2);
        static_assert(!has_set_all_to<any<mapable, anyxx::cref, int>>);
    }
    {
        std::vector<int> v1{ 1, 2 };
        auto m = using_<std::vector<int>>::as<mapable, int>(v1);
        decltype(auto) v = m.at(0);
        std::println("{}", v);
        static_assert(std::same_as<decltype(v), int&>);
        CHECK(m.at(0) == 1);
        CHECK(m.at(1) == 2);
        CHECK(m[1] == 2);
        CHECK(m.size() == 2);
        m.set_all_to(42);
        static_assert(has_set_all_to<any<mapable, anyxx::mutref, int>>);
    }
}

namespace{
template <is_trait Trait, typename Self, typename Value>
struct pointable_to {
    Value& operator*() const;
    Value* operator->() const;
    Value& operator[](std::size_t) const;
    Self& operator++();
    Self operator+(std::size_t) const;
//    bool operator==(Self const&) const;
};
}
TEST_CASE("anyxx26 pointable_to trait_as") {
    consteval{
        constexpr auto interface_specs_with_target = std::define_static_array(make_interface_specs_with_target<int*, pointable_to, int>());
        static_assert(interface_specs_with_target.size() == 5);
        using tt = trait_as<int*, pointable_to, int>;
        using r_type = trait_as_return_type_t<tt, interface_specs_with_target[3].member>;
        constexpr auto return_type = return_type_of(interface_specs_with_target[3].member);
        static_assert(!is_const_function(interface_specs_with_target[3].member));
        static_assert(return_type == ^^declaration&);
        static_assert(std::same_as<r_type, tt&>);
        using r_type1 = trait_as_return_type_t<tt, interface_specs_with_target[4].member>;
        static_assert(std::same_as<r_type1, tt>);
    }
    {
        std::array<int, 2> v1  = { 1, 2 };
        auto v1ptr = v1.data();
        auto const m = as<pointable_to, int>(v1ptr);
        decltype(auto) v = m[0];
        std::println("{}", v);
        static_assert(std::same_as<decltype(v), int&>);
        decltype(auto) v2 = *m;
        std::println("{}", v2);
        static_assert(std::same_as<decltype(v2), int&>);
        CHECK(*m == 1);
        CHECK(m[0] == 1);
        CHECK(m[1] == 2);
   //     auto m2 = m + 1;
        auto mm = as<pointable_to, int>(v1ptr);
        decltype(auto) mm1 = ++mm;
        static_assert(std::same_as<decltype(mm1), trait_as<int*, pointable_to, int>&>);
        CHECK(*mm1 == 2);
        //CHECK(++m == m);
        //CHECK(*m == 2);
    }
    {
        std::array<std::string, 2> v1  = { "1", "2" };
        auto const m = as<pointable_to, std::string>(v1.data());
        decltype(auto) v = m[0];
        std::println("{}", v);
        static_assert(std::same_as<decltype(v), std::string&>);
        CHECK(m[0] == "1");
        CHECK(m[1] == "2");
        CHECK(m->size() == 1u);
    }
}
