#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/copy_convert.hpp>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/data/unique_ptr.hpp>
#include <virtual_void/data/value.hpp>
#include <virtual_void/data/move_convert.hpp>
#include <virtual_void/interface/declare_macro.hpp>

#include "a.hpp"
#include "class_hierarchy_test_hierarchy.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;
using namespace virtual_void::interface;
using namespace virtual_void::meta;
using namespace TestDomain;

VV_RUNTIME_STATIC(type_info, std::string)

TEST_CASE("_data_conversion copy") {
  std::string s1 = "hallo";
  auto& runtime = meta::runtime<meta::type_info, std::string>();
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<mutable_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<mutable_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<mutable_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<shared_const>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<unique>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<unique>(s1);
    auto vv2 = data::copy_convert_to<mutable_observer>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<value>(s1);
    auto vv2 = data::copy_convert_to<const_observer>(vv1, runtime);
    REQUIRE(*unchecked_unerase_cast<std::string>(vv1) == *unchecked_unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<value>(s1);
    auto vv2 = data::copy_convert_to<mutable_observer>(vv1, runtime);
    REQUIRE(*unchecked_unerase_cast<std::string>(vv1) == *unchecked_unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<const_observer>(s1);
    auto vv2 = data::copy_convert_to<unique>(vv1, runtime);
    auto sc1 = unchecked_unerase_cast<std::string>(vv1);
    auto sc2 = unchecked_unerase_cast<std::string>(vv2);
    REQUIRE(*sc1 == *sc2);
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
  {
    auto vv1 = erased<shared_const>(s1);
    auto vv2 = data::copy_convert_to<shared_const>(vv1, runtime);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<value>(s1);
    auto vv2 = data::copy_convert_to<value>(vv1, runtime);
    auto sc1 = unchecked_unerase_cast<std::string>(vv1);
    auto sc2 = unchecked_unerase_cast<std::string>(vv2);
    REQUIRE(*sc1 == *sc2);
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
  {
    auto vv1 = erased<shared_const>(s1);
    auto vv2 = data::copy_convert_to<unique>(vv1, runtime);
    REQUIRE(*unchecked_unerase_cast<std::string>(vv1) == *unchecked_unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
  {
    auto vv1 = erased<value>(s1);
    auto vv2 = data::copy_convert_to<unique>(vv1, runtime);
    REQUIRE(*unchecked_unerase_cast<std::string>(vv1) == *unchecked_unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
  {
    auto vv1 = erased<unique>(s1);
    auto vv2 = data::copy_convert_to<unique>(vv1, runtime);
    REQUIRE(*unchecked_unerase_cast<std::string>(vv1) == *unchecked_unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
}
TEST_CASE("_data_conversion move") {
  std::string s1 = "hallo";
  {
    auto vv1 = erased<unique>(s1);
    auto vv2 = data::move_convert_to<unique>(std::move(vv1));
    REQUIRE(s1 == *unchecked_unerase_cast<std::string>(vv2));
#pragma warning( push )
#pragma warning( disable : 26800)
    REQUIRE(get_data(vv1) != get_data(vv2));
    REQUIRE(!has_data(vv1));
#pragma warning( pop )
  }
  {
    auto vv1 = erased<unique>(s1);
    auto vv2 = data::move_convert_to<shared_const>(std::move(vv1));
    REQUIRE(s1 == *unchecked_unerase_cast<std::string>(vv2));
#pragma warning( push )
#pragma warning( disable : 26800)
    REQUIRE(get_data(vv1) != get_data(vv2));
    REQUIRE(!has_data(vv1));
#pragma warning( pop )
  }
}