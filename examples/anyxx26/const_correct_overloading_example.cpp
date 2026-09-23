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

TEST_CASE("anyxx26 const correctness overloading trait_as") {
    {
        std::vector<int> v1{ 1, 2 };
        auto m = using_<std::vector<int>>::as<mapable, int const>(v1);
        decltype(auto) v = m.at(0);
        std::println("{}", v);
        static_assert(std::same_as<decltype(v), int const&>);
        CHECK(m.at(0) == 1);
        CHECK(m.at(1) == 2);
        //CHECK(m[1] == 2);
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
        //CHECK(m[1] == 2);
        m.set_all_to(42);
        static_assert(has_set_all_to<any<mapable, anyxx::mutref, int>>);
    }
}
