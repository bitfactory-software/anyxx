#include <catch2/catch_test_macros.hpp>
#include <bit_factory/v26/any/function.hpp>
#include <bit_factory/v26/meta/print_members.hpp>

using namespace anyxx26;

namespace {

template <typename Self, typename, typename R, typename... Args>
struct test_function_named_defaulted : copyable<Self, declaration> {
    [[= defaulted]] static R op_parentheses(Self const& self, Args... args);
};

template <typename Self, typename, typename R, typename... Args>
struct test_function_named : copyable<Self, declaration> {
    static R op_parentheses(Self const& self, Args... args) {
        return self(std::forward<Args>(args)...);
    }
};

//template <typename Self, typename, typename R, typename... Args>
//struct mutable_copyable_function : copyable<Self, declaration> {
//    static R op_parentheses(Self const& self, Args... args) {
//        return self(std::forward<Args>(args)...);
//    }
//};

template <typename Spec, typename V>
void test_default_impl(V const& v) {
    constexpr auto spec_f = meta::get_member_by_function_name(^^Spec, std::define_static_string("op_parentheses"));
    static_assert(spec_f != std::meta::info{});
    auto r = default_impl<spec_f, V, int, const void*, int>()(&v, 2);
    CHECK(r == 4);
}

struct callable_test{
    int operator()(int i) const {
        return i * i;
    }
};

}

TEST_CASE("anyxx26 std function equivalents") {

    auto lambda = +[](int x){ return x + 2; };
    auto lambda2 = [](int x){ return x + 2; };
    callable_test callable_object;

    test_default_impl<const_copyable_function<void*, declaration, int, int>>(lambda);
    test_default_impl<const_copyable_function<void*, declaration, int, int>>(lambda2);
    test_default_impl<const_copyable_function<void*, declaration, int, int>>(callable_object);

    {
        //dyn<const_copyable_function, anyxx::val<>, int, int> f{ [](int x){ return x + 2; } };
        dyn<const_copyable_function, anyxx::val<>, int, int> f{ lambda2 };
        //anyxx26::meta::print_members<const_copyable_function<void*, declaration, int, int>>();
        //anyxx26::meta::print_members<decltype(lambda)>();
        //anyxx26::meta::print_members<decltype(lambda2)>();
        //anyxx26::meta::print_members<decltype(callable_object)>();
        //anyxx26::meta::print_members<decltype(f)>();
        CHECK(f.op_parentheses(40) == 42);
        CHECK(f(40) == 42);
        auto f2 = f;
        CHECK(f.op_parentheses(0) == 2);
    }
    {
        //dyn<const_copyable_function, anyxx::val<>, int, int> f{ [](int x){ return x + 2; } };
        dyn<test_function_named_defaulted, anyxx::val<>, int, int> f{ lambda2 };
        //anyxx26::meta::print_members<const_copyable_function<void*, declaration, int, int>>();
        //anyxx26::meta::print_members<decltype(lambda)>();
        //anyxx26::meta::print_members<decltype(lambda2)>();
        //anyxx26::meta::print_members<decltype(callable_object)>();
        //anyxx26::meta::print_members<decltype(f)>();
        CHECK(f.op_parentheses(40) == 42);
        CHECK(f(40) == 42);
        auto f2 = f;
        CHECK(f.op_parentheses(0) == 2);
    }
    {
        //dyn<const_copyable_function, anyxx::val<>, int, int> f{ [](int x){ return x + 2; } };
        dyn<test_function_named, anyxx::val<>, int, int> f{ lambda2 };
        //anyxx26::meta::print_members<const_copyable_function<void*, declaration, int, int>>();
        //anyxx26::meta::print_members<decltype(lambda)>();
        //anyxx26::meta::print_members<decltype(lambda2)>();
        anyxx26::meta::print_members<decltype(callable_object)>();
        //anyxx26::meta::print_members<decltype(f)>();
        CHECK(f.op_parentheses(40) == 42);
        CHECK(f(40) == 42);
        auto f2 = f;
        CHECK(f.op_parentheses(0) == 2);
    }
    {
        copyable_function<int(int) const> f{ [](int x){ return x + 2; } };
        CHECK(f.op_parentheses(40) == 42);
        CHECK(f(40) == 42);
        auto f2 = f;
        CHECK(f.op_parentheses(0) == 2);
    }
}
