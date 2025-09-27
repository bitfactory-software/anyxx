#include <any>
#include <catch.hpp>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <virtual_void/data/decorated_data.hpp>
#include <virtual_void/data/make_shared_const_decorated_data.hpp>
#include <virtual_void/data/make_unique_decorated_data.hpp>
#include <virtual_void/data/make_value_decorated_data.hpp>
#include <virtual_void/meta/class.hpp>
#include <virtual_void/virtual_void.hpp>

using namespace Catch::Matchers;

using namespace virtual_void;

#define DATA_ALIGNED(T) data::decorated_data<T>

#define ASSERT_OFFSET_EMPTY(T, o) \
  static_assert(offsetof(DATA_ALIGNED(T), value_) == o);

#define ASSERT_OFFSET(T, o) \
  static_assert(offsetof(DATA_ALIGNED(T), value_) == o);

#ifdef _DEBUG
#define OFFSET_FOR_V_TABLE 8u
#else
#define OFFSET_FOR_V_TABLE 0u
#endif
//static const constexpr std::size_t offset_for_v_table = OFFSET_FOR_V_TABLE;

//ASSERT_OFFSET_EMPTY(char, offset_for_v_table);
//ASSERT_OFFSET_EMPTY(int, offset_for_v_table);
//ASSERT_OFFSET_EMPTY(char const*, offset_for_v_table)
//ASSERT_OFFSET_EMPTY(std::string, offset_for_v_table)

#define TRACE_OFFSET_EMPTY(T)                                     \
  {                                                               \
    using TYPE = data::decorated_data<T>;                         \
    std::cout << "data::decorated_data<" << #T                    \
              << "> offsetof(value_): " << offsetof(TYPE, value_) \
              << std::endl;                                       \
  }

#define TRACE_OFFSET(T)                                           \
  {                                                               \
    using TYPE = data::decorated_data<T>;                         \
    std::cout << "data::decorated_data<" << #T                    \
              << "> offsetof(value_): " << offsetof(TYPE, value_) \
              << std::endl;                                       \
  }
//              << ", offsetof(meta_data_): " << offsetof(TYPE, meta_data_) \

struct Data {
  static int destrucor_runs;
  std::string s_ = "hello world";
  ~Data() { ++destrucor_runs; }
};
int Data::destrucor_runs = 0;

VV_RUNTIME_STATIC(type_info, Data)

TEST_CASE("erase lifetime test") {
  TRACE_OFFSET_EMPTY(char);
  TRACE_OFFSET_EMPTY(int);
  TRACE_OFFSET_EMPTY(char const*);
  TRACE_OFFSET_EMPTY(std::string);

  TRACE_OFFSET(char);
  TRACE_OFFSET(int);
  TRACE_OFFSET(char const*);
  TRACE_OFFSET(std::string);
}
TEST_CASE("erase lifetime test unique") {
  Data::destrucor_runs = 0;
  {
    auto unique_ptr = data::make_unique_decorated_data<
        data::decorated_data<Data>>();
    REQUIRE(unchecked_unerase_cast<Data>(*unique_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    auto unique_ptr = data::make_unique_decorated_data<
        data::decorated_data<Data>>();
    REQUIRE(unchecked_unerase_cast<Data>(*unique_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetime test shared") {
  Data::destrucor_runs = 0;
  {
    std::shared_ptr<data::decoration_base const> sp =
        data::make_shared_const_decorated_data<
            data::decorated_data<Data>>();
    REQUIRE(data::unchecked_unerase_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    std::shared_ptr<data::decoration_base const> sp =
        data::make_shared_const_decorated_data<
            data::decorated_data<Data> const>();
    REQUIRE(data::unchecked_unerase_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetime test value") {
  Data::destrucor_runs = 0;
  {
    data::erased_value<data::decoration_base> vp =
        data::make_value_decorated_data<
            data::decorated_data<Data>>();
    REQUIRE(unchecked_unerase_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);

  Data::destrucor_runs = 0;
  {
    data::erased_value<data::decoration_base> vp =
        data::make_value_decorated_data<
            data::decorated_data<Data>>();
    REQUIRE(data::unchecked_unerase_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);
}
