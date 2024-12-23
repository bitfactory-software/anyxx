#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/copy_convert.hpp>
#include <virtual_void/data/has_meta_runtime/observer.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const.hpp>
#include <virtual_void/data/has_meta_runtime/shared_const_ptr.hpp>
#include <virtual_void/data/has_meta_runtime/unique.hpp>
#include <virtual_void/data/has_meta_runtime/unique_ptr.hpp>
#include <virtual_void/data/has_meta_runtime/value.hpp>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/data/has_no_meta/shared_const.hpp>
#include <virtual_void/data/has_no_meta/shared_const_ptr.hpp>
#include <virtual_void/data/has_no_meta/unique.hpp>
#include <virtual_void/data/has_no_meta/unique_ptr.hpp>
#include <virtual_void/data/has_no_meta/value.hpp>
#include <virtual_void/data/move_convert.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/meta/std_runtime.hpp>
#include <virtual_void/meta/type_info.hpp>

#include "a.hpp"
#include "class_hierarchy_test_hierarchy.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::data;
using namespace virtual_void::interface;
using namespace virtual_void::meta;
using namespace TestDomain;


TEST_CASE("_data_conversion copy") {
  std::string s1 = "hallo";
  {
    auto vv1 = erased<has_no_meta::const_observer>(s1);
    auto vv2 = data::copy_convert_to<has_no_meta::const_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_no_meta::mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<has_no_meta::const_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_no_meta::mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<has_no_meta::mutable_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::const_observer>(s1);
    auto vv2 = data::copy_convert_to<has_no_meta::const_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<has_no_meta::const_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<has_no_meta::mutable_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::const_observer>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::const_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::const_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::mutable_observer>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::mutable_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::shared_const>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::const_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::unique>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::const_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::unique>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::mutable_observer>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::value>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::const_observer>(vv1);
    REQUIRE(*unerase_cast<std::string>(vv1) == *unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::value>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::mutable_observer>(vv1);
    REQUIRE(*unerase_cast<std::string>(vv1) == *unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::const_observer>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::unique>(vv1);
    auto sc1 = unerase_cast<std::string>(vv1);
    auto sc2 = unerase_cast<std::string>(vv2);
    REQUIRE(*sc1 == *sc2);
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::shared_const>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::shared_const>(vv1);
    REQUIRE(get_data(vv1) == get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::value>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::value>(vv1);
    auto sc1 = unerase_cast<std::string>(vv1);
    auto sc2 = unerase_cast<std::string>(vv2);
    REQUIRE(*sc1 == *sc2);
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::shared_const>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::unique>(vv1);
    REQUIRE(*unerase_cast<std::string>(vv1) == *unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::value>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::unique>(vv1);
    REQUIRE(*unerase_cast<std::string>(vv1) == *unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
  {
    auto vv1 = erased<has_meta_runtime::unique>(s1);
    auto vv2 = data::copy_convert_to<has_meta_runtime::unique>(vv1);
    REQUIRE(*unerase_cast<std::string>(vv1) == *unerase_cast<std::string>(vv2));
    REQUIRE(get_data(vv1) != get_data(vv2));
  }
}
TEST_CASE("_data_conversion move") {
  std::string s1 = "hallo";
  {
    auto vv1 = erased<has_meta_runtime::unique>(s1);
    auto vv2 = data::move_convert_to<has_meta_runtime::unique>(std::move(vv1));
    REQUIRE(s1 == *unerase_cast<std::string>(vv2));
#pragma warning( push )
#pragma warning( disable : 26800)
    REQUIRE(get_data(vv1) != get_data(vv2));
    REQUIRE(!has_data(vv1));
#pragma warning( pop )
  }
  {
    auto vv1 = erased<has_meta_runtime::unique>(s1);
    auto vv2 = data::move_convert_to<has_meta_runtime::shared_const>(std::move(vv1));
    REQUIRE(s1 == *unerase_cast<std::string>(vv2));
#pragma warning( push )
#pragma warning( disable : 26800)
    REQUIRE(get_data(vv1) != get_data(vv2));
    REQUIRE(!has_data(vv1));
#pragma warning( pop )
  }
}