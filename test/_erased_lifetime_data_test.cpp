#include <any>
#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/data/unique.hpp>
#include <virtual_void/runtime/meta_data.hpp>
#include <virtual_void/virtual_void.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;

struct Data {
  static int destrucor_runs;
  std::string s_ = "hello world";
  ~Data() { ++destrucor_runs; }
};
int Data::destrucor_runs = 0;

VV_RUNTIME_STATIC(Data)

TEST_CASE("erase lifetime test unique") {
  Data::destrucor_runs = 0;
  {
    auto unique_ptr = data::make_unique<Data>();
    REQUIRE(unchecked_unerase_cast<Data>(unique_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    auto unique_ptr = data::make_unique<Data>();
    REQUIRE(unchecked_unerase_cast<Data>(unique_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetime test shared") {
  Data::destrucor_runs = 0;
  {
    data::shared_const sp = std::make_shared<Data>();
    REQUIRE(unchecked_unerase_cast<Data>(sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    data::shared_const sp = std::make_shared<Data>();
    REQUIRE(unchecked_unerase_cast<Data>(sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
