#include <bit_factory/v26/any.hpp>
#include <catch2/catch_test_macros.hpp>
#include <any>
#include <typeindex>

using namespace anyxx26;

TEST_CASE("anyxx26 std any equivalents") {
  {
    std::any a1 = 42;
    CHECK(*std::any_cast<int>(&a1) == 42);
    CHECK(std::type_index(a1.type()) == std::type_index(typeid(int)));
    CHECK(!std::any_cast<double>(&a1));
    std::any a2 = a1;
    CHECK(*std::any_cast<int>(&a2) == 42);
    a1 = std::string{"hello"};
    CHECK(*std::any_cast<std::string>(&a1) == "hello");
    CHECK(*std::any_cast<int>(&a2) == 42);
  }
  {
    auto a1 = any<save_copyable>{42};
    CHECK(*unerase_cast<int>(a1) == 42);
    CHECK(std::type_index(anyxx::get_type_info(a1)) == std::type_index(typeid(int)));
    CHECK(!unerase_cast<double>(a1));
    any<save_copyable> a2 = a1;
    CHECK(*unerase_cast<int>(a2) == 42);
    a1 = std::string{"hello"};
    CHECK(*unerase_cast<std::string>(a1) == "hello");
    CHECK(*unerase_cast<int>(a2) == 42);
  }

  {
    auto a1 = any<save_moveable>{42};
    static_assert(!std::copy_constructible<any<save_moveable>>);
    CHECK(*unerase_cast<int>(a1) == 42);
    any<save_moveable> a2 = std::move(a1);
    CHECK(*unerase_cast<int>(a2) == 42);
  }
}
