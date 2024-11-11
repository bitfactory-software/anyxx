#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "include/catch.hpp"

using namespace Catch::Matchers;

import virtual_void;
//#include "virtual_void/open_object/members.h"

using namespace virtual_void;
using namespace virtual_void::open_object;

struct test_object : members<test_object> {};  // name your open_object

struct test_member : member<test_object, test_member, std::string> {
};  // define your meber

const member_table_index<test_object>
    member<test_object, test_member, std::string>::
        index;  // declare the static index, witch is used by "mebers" to access
                // this member. This static variable must be reachable from each
                // translation unit, that wants to access the memebers from
                // test_object for each accces

TEST_CASE("open object 1") {
  REQUIRE(open_object::type_member_count_of<test_object>() == 1);
  test_object a_test_object;
  auto value = a_test_object.get(test_member());
  REQUIRE(!value);
  a_test_object.set(test_member(), "hello world");
  auto r = a_test_object.get(test_member{});
  REQUIRE(r);
  std::cout << typeid(*r).name() << "\n";
  REQUIRE(*r == "hello world");
  static_assert(std::same_as<decltype(r), std::string*>);
  REQUIRE((*a_test_object.get(test_member())) == "hello world");
  {
    test_object const& const_test_object = a_test_object;
    auto r = const_test_object.get(test_member{});
    REQUIRE(r);
    std::cout << typeid(*r).name() << "\n";
    REQUIRE(*r == "hello world");
    static_assert(std::same_as<decltype(r), std::string const*>);
    REQUIRE((*const_test_object.get(test_member())) == "hello world");
  }
  *a_test_object.get(test_member()) = "hello programm";
  REQUIRE((*a_test_object.get(test_member())) == "hello programm");
}
TEST_CASE("open object 2") {
  test_object a_test_object;
  auto r = a_test_object.get(test_member{});
  REQUIRE(!r);
  a_test_object[test_member()] = "hello world";
  REQUIRE(a_test_object[test_member()] == "hello world");
}
TEST_CASE("open object 3") {
  test_object a_test_object;
  auto r = a_test_object.get(test_member{});
  REQUIRE(!r);
  REQUIRE(a_test_object[test_member()] == "");
}