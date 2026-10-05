#include <bit_factory/v26/anys/function.hpp>
#include <bit_factory/v26/meta/print_members.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace anyxx26;

namespace {

struct callable_test {
  int operator()(int i) const { return i * i; }
};

template <typename Func>
concept is_callable = requires(Func const& f, int i) {
  { f(i) } -> std::same_as<int>;
};

}  // namespace

TEST_CASE("anyxx26 std function equivalents") {
  {
    any<function, int(int) const> f{ [](int x) { return x + 2; }};
    CHECK(f(40) == 42);
  }
  {
    any<function, int(int)> f{[](int x) { return x + 2; }};
    CHECK(f(40) == 42);
  }
  {
    any<function, int(int)> f{[y = 1](int x) mutable{ return x + ++y + 2; }};
    CHECK(f(40) == 44);
    CHECK(f(40) == 45);
  }
  {
      // +++ this code does not compile, as expected
      //any<function, int(int) const> f{[y = 1](int x) mutable{ return x + ++y + 2; }};
      // -- this code does not compile, as expected
  }
  {
    auto lambda = +[](int x) { return x + 2; };
    using function = any<function, const_referenceable, int(int) const>;
    function f{lambda};
    CHECK(f(40) == 42);
  }
  {
    auto const_lambda = +[](int x) { return x + 2; };
    auto mutable_lambda = [&](int x) mutable { return x + 2; };
    using function = any<function, mutable_referenceable, int(int)>;
    function f_mutable_lambda{mutable_lambda};
    function f_const_lambda{const_lambda};
    static_assert(std::is_invocable_v<decltype(f_mutable_lambda), int>);
    static_assert(std::is_invocable_v<decltype(f_const_lambda), int>);
    f_mutable_lambda(1);
    f_const_lambda(1);
    // +++ this code does not compile, as expected
    // const auto& cref_f_const_lambda = f_const_lambda;
    // const auto& cref_f_mutable_lambda = f_mutable_lambda;
    // cref_f_const_lambda(1);
    // cref_f_mutable_lambda(1);
    // ---

    CHECK(f_mutable_lambda(40) == 42);
    auto f2 = f_mutable_lambda;
    CHECK(f_mutable_lambda(0) == 2);
  }
  {
    [[maybe_unused]] auto mutable_lambda = [&](int x) mutable { return x + 2; };
    // +++ this code does not compile, as expected
    // function f_mutable_lambda{ mutable_lambda };
    // ---
    auto const_lambda = [](int x) { return x + 2; };
    using function = any<function, mutable_referenceable, int(int) const>;
    function f_const_lambda{const_lambda};
    static_assert(std::is_invocable_v<decltype(f_const_lambda), int>);
    f_const_lambda(1);
    const auto& cref_f_const_lambda = f_const_lambda;
    static_assert(std::is_invocable_v<decltype(cref_f_const_lambda), int>);
    cref_f_const_lambda(1);

    CHECK(f_const_lambda(40) == 42);
    auto f2 = f_const_lambda;
    CHECK(f_const_lambda(0) == 2);
  }
  {
    auto const_lambda = [](int x) { return x + 2; };
    [[maybe_unused]] auto mutable_lambda = [&](int x) mutable { return x + 2; };
    using function = any<function, const_referenceable, int(int) const>;
    // +++ this code does not compile, as expected
    //[[maybe_unused]] function f_mutable_lambda{ mutable_lambda };
    // ---
    function f_const_lambda{const_lambda};
    // static_assert(!std::is_invocable_v<decltype(f_mutable_lambda), int>);
    static_assert(std::is_invocable_v<decltype(f_const_lambda), int>);
    CHECK(f_const_lambda(1) == 3);
  }
  {
    auto p1 = std::make_unique<int>(42);
    using function = any<function, moveable<>, int(int)>;
    function f_moveable{[p = std::move(p1)](int x) { return x + ++*p; }};
    static_assert(!std::is_copy_constructible_v<function>);
    static_assert(std::is_move_constructible_v<function>);
    CHECK(f_moveable(1) == 44);
    function f_moveable1{std::move(f_moveable)};
    CHECK(f_moveable1(1) == 45);
  }
}
