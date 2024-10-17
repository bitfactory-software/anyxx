#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/open_object/open_object.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;

struct test_object : open_object::members<test_object> {};

struct test_member
    : open_object::member<test_object, test_member, std::string> {};

const open_object::member_table_index<test_object>
    open_object::member<test_object, test_member, std::string>::index;

TEST_CASE("open object") {
  REQUIRE(open_object::type_member_count_of<test_object>() == 1);
  test_object a_test_object;
  auto value = a_test_object.get(test_member());
  REQUIRE(!value);
  a_test_object.set(test_member(), std::string("hello world"));
  auto r = a_test_object.get(test_member{});
  REQUIRE(r);
  std::cout << typeid(*r).name() << "\n";
  REQUIRE(*r == "hello world");
  static_assert(std::same_as<decltype(r), std::string const*>);
  REQUIRE((*a_test_object.get(test_member())) == "hello world");
}