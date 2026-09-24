#include <catch2/catch_test_macros.hpp>
#include <bit_factory/v26/anys/function.hpp>
#include <bit_factory/v26/meta/print_members.hpp>
#include <functional>

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

        test_default_impl<function<declaration, declaration, copyable<>, int(int) const>>(lambda);
        test_default_impl<function<declaration, declaration, copyable<>, int(int) const>>(lambda2);
        test_default_impl<function<declaration, declaration, copyable<>, int(int) const>>(callable_object);
       }
    {
        auto lambda2 = [](int x){ return x + 2; };
        //any<const_copyable_function, anyxx::val<>, int, int> f{ [](int x){ return x + 2; } };
        any<function, int(int) const> f{ lambda2 };
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
        //any<const_copyable_function, anyxx::val<>, int, int> f{ [](int x){ return x + 2; } };
        any<test_function_named_defaulted, int, int> f{ lambda2 };
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
        //any<const_copyable_function, anyxx::val<>, int, int> f{ [](int x){ return x + 2; } };
        any<test_function_named, int, int> f{ lambda2 };
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
        any<function, copyable<>, int(int) const> f{ [](int x){ return x + 2; } };
        CHECK(f.op_parentheses(40) == 42);
        CHECK(f(40) == 42);
        auto f2 = f;
        CHECK(f.op_parentheses(0) == 2);
    }
    {
        auto lambda = +[](int x){ return x + 2; };
        using function = any<function, const_referenceable, int(int) const>;
        function f{ lambda };

        CHECK(f.op_parentheses(40) == 42);
        CHECK(f(40) == 42);
        auto f2 = f;
        CHECK(f.op_parentheses(0) == 2);
    }
    {
        auto const_lambda = +[](int x){ return x + 2; };
        auto mutable_lambda = [&](int x) mutable { return x + 2; };
        using function = any<function, mutable_referenceable, int(int)>;
        function f_mutable_lambda{mutable_lambda};
        function f_const_lambda{const_lambda};
        static_assert(std::is_invocable_v<decltype(f_mutable_lambda), int>);
        static_assert(std::is_invocable_v<decltype(f_const_lambda), int>);
        f_mutable_lambda(1);
        f_const_lambda(1);
        // +++ this code does not compile, as expected
        //const auto& cref_f_const_lambda = f_const_lambda;
        //const auto& cref_f_mutable_lambda = f_mutable_lambda;
        //cref_f_const_lambda(1);
        //cref_f_mutable_lambda(1);
        // ---

        CHECK(f_mutable_lambda.op_parentheses(40) == 42);
        CHECK(f_mutable_lambda(40) == 42);
        auto f2 = f_mutable_lambda;
        CHECK(f_mutable_lambda.op_parentheses(0) == 2);
    }
    {
        [[maybe_unused]] auto mutable_lambda = [&](int x) mutable { return x + 2; };
        // +++ this code does not compile, as expected
        //function f_mutable_lambda{ mutable_lambda };
        // ---
        auto const_lambda = [](int x){ return x + 2; };
        using function = any<function, mutable_referenceable, int(int) const>;
        function f_const_lambda{ const_lambda };
        static_assert(std::is_invocable_v<decltype(f_const_lambda), int>);
        f_const_lambda(1);
        const auto& cref_f_const_lambda = f_const_lambda;
        static_assert(std::is_invocable_v<decltype(cref_f_const_lambda), int>);
        cref_f_const_lambda(1);

        CHECK(f_const_lambda.op_parentheses(40) == 42);
        CHECK(f_const_lambda(40) == 42);
        auto f2 = f_const_lambda;
        CHECK(f_const_lambda.op_parentheses(0) == 2);
    }
    {
        [[maybe_unused]] auto const_lambda = [](int x){ return x + 2; };
        [[maybe_unused]] auto mutable_lambda = [&](int x) mutable { return x + 2; };
        using function = any<function, const_referenceable, int(int) const>;
        // +++ this code does not compile, as expected
        //[[maybe_unused]] function f_mutable_lambda{ mutable_lambda };
        // ---
        [[maybe_unused]] function f_const_lambda{ const_lambda };
        //static_assert(!std::is_invocable_v<decltype(f_mutable_lambda), int>);
        static_assert(std::is_invocable_v<decltype(f_const_lambda), int>);
        CHECK(f_const_lambda(1) == 3);
    }

    any<function, copyable<>, int(void) const> f{[]() { return 2; }};
}

namespace dummy {

template <typename R, typename... Args>
using signature = R(Args...);
template <typename R, typename... Args>
using signature_const = R(Args...) const;

template <typename... Args>
consteval std::meta::info make_function_signature(bool const_){
    auto signature_params = std::vector{^^Args...} | std::views::drop(3);
    if (const_) {
        return substitute(^^signature_const, signature_params);
    } else {
        return substitute(^^signature, signature_params);
    }
}

static_assert(dealias(make_function_signature<declaration, declaration, void*, int, double, bool>(false)) ==
              ^^int(double, bool));
//static_assert(std::same_as<typename [:make_function_signature<declaration, declaration, void*, int, double, bool>(false):],
//    int(double, bool));

template <typename FirstRest, typename... RestArgs>
struct extract_signature {
    template <typename... ResultArgs>
    struct result;
    template <typename... ResultArgs>
      requires(sizeof...(RestArgs) == 1)
    struct result<ResultArgs...>{
      template <typename R>
        using type = R(ResultArgs...);
    };
    template <typename... ResultArgs>
        requires(sizeof...(RestArgs) > 1)
    struct result<ResultArgs...> : extract_signature<RestArgs...>::template result<ResultArgs..., FirstRest>{};
};


using sig_t = int(int);
using sig1_t = extract_signature<int, void, void>::template result<>::type<int>;
static_assert(std::same_as<sig_t, sig1_t>);
//template <typename R, typename... Args>
//using x_function = std::function<extract_signature<Args...>::template result<>::type<R>>;
//static_assert(std::same_as<x_function<int, double, bool, bool>, std::function<int(double)>>);
//using f = extract_signature<double, bool, bool>::template result<>::type<int>;
static_assert(std::same_as<std::function<extract_signature<double, bool, bool>::template result<>::type<int>>, std::function<int(double)>>);

TEST_CASE("anyxx26 function signature") {
  std::function<sig_t> f = [](auto i) { return i + 1;};
  CHECK(f(1) == 2);
}

template <is_trait Trait, typename Self, typename Base, typename R, typename... Args>
struct function;

template <is_trait Trait, typename Self, typename Base, typename R, typename... Args>
struct function<Trait, Self, Base, R(Args...) const> : Base::template self_apply<Trait, Self> {
    R operator()(Args... args) const;
};
template <is_trait Trait, typename Self, typename Base, typename R, typename... Args>
struct function<Trait, Self, Base, R(Args...)> : Base::template self_apply<Trait, Self> {
    R operator()(Args... args);
};

};