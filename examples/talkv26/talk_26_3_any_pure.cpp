#include <any>
#include <bit_factory/anyxx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <typeindex>

TEST_CASE("talk_23_3_any_pure") {
  {
    using namespace std;
    any a1 = 42;
    CHECK(*any_cast<int>(&a1) == 42);
    CHECK(std::type_index(a1.type()) == std::type_index(typeid(int)));
    CHECK(!any_cast<double>(&a1));
    any a2 = a1;
    CHECK(*any_cast<int>(&a2) == 42);
    a1 = std::string{"hello"};
    CHECK(*any_cast<std::string>(&a1) == "hello");
    CHECK(*any_cast<int>(&a2) == 42);
  }
  {
    using namespace anyxx;
    auto a1 = any<typed_copyable>{42};
    CHECK(any_cast<int>(a1) == 42);
    CHECK(std::type_index(type_of(a1)) == std::type_index(typeid(int)));
    CHECK(!any_cast_if<double>(a1));
    any<typed_copyable> a2 = a1;
    CHECK(any_cast<int>(a2) == 42);
    a1 = std::string{"hello"};
    CHECK(any_cast<std::string>(a1) == "hello");
    CHECK(any_cast<int>(a2) == 42);
  }
  {
    using namespace anyxx;
    auto a1 = any<typed_moveable>{42};
    static_assert(!std::copy_constructible<any<typed_moveable>>);
    static_assert(std::move_constructible<any<typed_moveable>>);
    CHECK(any_cast<int>(a1) == 42);
    any<typed_moveable> a2 = std::move(a1);
    CHECK(any_cast<int>(a2) == 42);
  }
}
