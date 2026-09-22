#include <catch2/catch_test_macros.hpp>
#include <bit_factory/v26/any/function.hpp>
#include <bit_factory/v26/meta/print_members.hpp>

using namespace anyxx26;

namespace {

template <is_trait Trait, typename Self, typename R, typename... Args>
struct test_function_named_defaulted : copyable<Trait, Self> {
    [[= defaulted]] static R op_parentheses(Self const& self, Args... args);
};

template <is_trait Trait, typename Self, typename R, typename... Args>
struct test_function_named : copyable<Trait, Self> {
    static R op_parentheses(Self const& self, Args... args) {
        return self(std::forward<Args>(args)...);
    }
};

//template <typename Self, typename, typename R, typename... Args>
//struct mutable_copyable_function : copyable<Trait, Self> {
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

template <typename Func>
concept is_callable =
    requires(Func const& f, int i) {
      { f(i) } -> std::same_as<int>;
    };

}

TEST_CASE("anyxx26 std function equivalents") {

    {
        auto lambda = +[](int x){ return x + 2; };
        auto lambda2 = [](int x){ return x + 2; };
        callable_test callable_object;

        test_default_impl<function<declaration, declaration, base<copyable>, int(int) const>>(lambda);
        test_default_impl<function<declaration, declaration, base<copyable>, int(int) const>>(lambda2);
        test_default_impl<function<declaration, declaration, base<copyable>, int(int) const>>(callable_object);
       }
    {
        auto lambda2 = [](int x){ return x + 2; };
        //dyn<const_copyable_function, anyxx::val<>, int, int> f{ [](int x){ return x + 2; } };
        dyn<function, base<copyable>, int(int) const> f{ lambda2 };
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
        auto lambda2 = [](int x){ return x + 2; };
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
        auto lambda2 = [](int x){ return x + 2; };
        callable_test callable_object;
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
        dyn<function, base<copyable>, int(int) const> f{ [](int x){ return x + 2; } };
        CHECK(f.op_parentheses(40) == 42);
        CHECK(f(40) == 42);
        auto f2 = f;
        CHECK(f.op_parentheses(0) == 2);
    }
    {
        auto lambda = +[](int x){ return x + 2; };
        using function = dyn<function, const_referenceable, int(int) const>;
        function f{ lambda };

        CHECK(f.op_parentheses(40) == 42);
        CHECK(f(40) == 42);
        auto f2 = f;
        CHECK(f.op_parentheses(0) == 2);
    }
    {
        auto const_lambda = +[](int x){ return x + 2; };
        auto mutable_lambda = [](int x) mutable { return x + 2; };
        using function = dyn<function, mutable_referenceable, int(int)>;
        function f_mutable{mutable_lambda};
        function f_const_lambda{const_lambda};
        static_assert(std::is_invocable_v<decltype(f_mutable), int>);
        static_assert(std::is_invocable_v<decltype(f_const_lambda), int>);
        f_mutable(1);
        f_const_lambda(1);
        // +++ this code does not compile, as expected
        //const auto& cref_f_const_lambda = f_const_lambda;
        //const auto& cref_f_mutable_lambda = f_mutable;
        //cref_f_const_lambda(1);
        //cref_f_mutable_lambda(1);
        // ---

        CHECK(f_mutable.op_parentheses(40) == 42);
        CHECK(f_mutable(40) == 42);
        auto f2 = f_mutable;
        CHECK(f_mutable.op_parentheses(0) == 2);
    }
    {
        //auto const_lambda = +[](int x){ return x + 2; };
        //auto mutable_lambda = [](int x) mutable { return x + 2; };
        //using function = dyn<function, mutable_referenceable, int(int) const>;
        //function f_mutable{ mutable_lambda };
        //function f_const_lambda{ const_lambda };
        //static_assert(std::is_invocable_v<decltype(f_mutable), int>);
        //static_assert(std::is_invocable_v<decltype(f_const_lambda), int>);
        //f_mutable(1);
        //f_const_lambda(1);
        //const auto& cref_f_const_lambda = f_const_lambda;
        //const auto& cref_f_mutable_lambda = f_mutable;
        ////static_assert(!std::is_invocable_v<decltype(cref_f_const_lambda), int>);
        ////static_assert(!std::is_invocable_v<decltype(cref_f_mutable_lambda), int>);
        //cref_f_const_lambda(1);
        //cref_f_mutable_lambda(1);

        //CHECK(f_mutable.op_parentheses(40) == 42);
        //CHECK(f_mutable(40) == 42);
        //auto f2 = f_mutable;
        //CHECK(f_mutable.op_parentheses(0) == 2);
    }
}
