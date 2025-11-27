#include <cmath>
#include <iostream>
#include <string>
#include <catch2/catch_test_macros.hpp>
#include <bit_factory/anyxx.hpp>

using namespace anyxx;

struct test_object;

struct test_object : members<test_object> {};  // name your open_object

inline const member<test_object, std::string> test_member;  // define your member

TEST_CASE("open object 1") {
  REQUIRE(members_count<test_object>() == 1);
  test_object a_test_object;
  auto value = a_test_object.get(test_member);
  REQUIRE(!value);
  a_test_object.set(test_member, "hello world");
  auto r = a_test_object.get(test_member);
  REQUIRE(r);
  std::cout << typeid(*r).name() << "\n";
  REQUIRE(*r == "hello world");
  static_assert(std::same_as<decltype(r), std::string*>);
  REQUIRE((*a_test_object.get(test_member)) == "hello world");
  {
    test_object const& const_test_object = a_test_object;
    auto r1 = const_test_object.get(test_member);
    REQUIRE(r);
    std::cout << typeid(*r).name() << "\n";
    REQUIRE(*r1 == "hello world");
    static_assert(std::same_as<decltype(r1), std::string const*>);
    REQUIRE((*const_test_object.get(test_member)) == "hello world");
  }
  *a_test_object.get(test_member) = "hello programm";
  REQUIRE((*a_test_object.get(test_member)) == "hello programm");
}
TEST_CASE("open object 2") {
  test_object a_test_object;
  auto r = a_test_object.get(test_member);
  REQUIRE(!r);
  a_test_object[test_member] = "hello world";
  REQUIRE(a_test_object[test_member] == "hello world");
}
TEST_CASE("open object 3") {
  test_object a_test_object;
  auto r = a_test_object.get(test_member);
  REQUIRE(!r);
  REQUIRE(a_test_object[test_member] == "");
}
TEST_CASE("test fails for test test") {
  REQUIRE(true); // for now
}
