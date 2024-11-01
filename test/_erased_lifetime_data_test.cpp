#include <any>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "virtual_void/data/decorated_data.h"
#include "virtual_void/data/has_no_meta/meta.h"
#include "virtual_void/data/shared_const_ptr.h"
#include "virtual_void/data/unique_ptr.h"
#include "virtual_void/data/value_ptr.h"
#include "virtual_void/virtual_void.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void;

namespace virtual_void::data {
}  // namespace virtual_void::data

#define DATA_ALIGNED(T, META_DATA) data::decorated_data<T, META_DATA>

#define ASSERT_OFFSET_EMPTY(T, o)                                             \
  static_assert(offsetof(DATA_ALIGNED(T, data::has_no_meta::meta), value_) == \
                o);

#define ASSERT_OFFSET(T, META_DATA, o) \
  static_assert(offsetof(DATA_ALIGNED(T, META_DATA), value_) == o);

#ifdef _DEBUG
#define OFFSET_FOR_V_TABLE 8u
#else
#define OFFSET_FOR_V_TABLE 0u
#endif
static const constexpr std::size_t offset_for_v_table = OFFSET_FOR_V_TABLE;

ASSERT_OFFSET_EMPTY(char, offset_for_v_table);
ASSERT_OFFSET_EMPTY(int, offset_for_v_table);
ASSERT_OFFSET_EMPTY(char const*, offset_for_v_table)
ASSERT_OFFSET_EMPTY(std::string, offset_for_v_table)

ASSERT_OFFSET(char, data::has_type_info::meta, 8 + offset_for_v_table);
ASSERT_OFFSET(int, data::has_type_info::meta, 8 + offset_for_v_table);
ASSERT_OFFSET(char const*, data::has_type_info::meta, 8 + offset_for_v_table);
ASSERT_OFFSET(std::string, data::has_type_info::meta, 8 + offset_for_v_table);

#define TRACE_OFFSET_EMPTY(T)                                         \
  {                                                                   \
    using TYPE = data::decorated_data<T, data::has_no_meta::meta>; \
    std::cout << "data::decorated_data<" << #T                     \
              << "> offsetof(value_): " << offsetof(TYPE, value_)     \
              << std::endl;                                           \
  }

#define TRACE_OFFSET(T, META_DATA)                                \
  {                                                               \
    using TYPE = data::decorated_data<T, META_DATA>;           \
    std::cout << "data::decorated_data<" << #T << ", " << #META_DATA      \
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

TEST_CASE("erase lifetime test") {
  TRACE_OFFSET_EMPTY(char);
  TRACE_OFFSET_EMPTY(int);
  TRACE_OFFSET_EMPTY(char const*);
  TRACE_OFFSET_EMPTY(std::string);

  TRACE_OFFSET(char, data::has_type_info::meta);
  TRACE_OFFSET(int, data::has_type_info::meta);
  TRACE_OFFSET(char const*, data::has_type_info::meta);
  TRACE_OFFSET(std::string, data::has_type_info::meta);
}
TEST_CASE("erase lifetime test unique") {
  Data::destrucor_runs = 0;
  {
    auto unique_ptr = data::make_unique<
        data::decorated_data<Data, data::has_no_meta::meta>>();
    REQUIRE(unerase_cast<Data>(*unique_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    auto unique_ptr = data::make_unique<
        data::decorated_data<Data, data::has_type_info::meta>>();
    REQUIRE(unerase_cast<Data>(*unique_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetime test shared") {
  Data::destrucor_runs = 0;
  {
    std::shared_ptr<data::decoration_base<data::has_no_meta::meta> const> sp =
        data::make_shared_const<data::decorated_data<Data, data::has_no_meta::meta>>();
    REQUIRE(data::unerase_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    std::shared_ptr<data::decoration_base<data::has_type_info::meta> const> sp =
        data::make_shared_const<
            data::decorated_data<Data, data::has_type_info::meta> const>();
    REQUIRE(data::unerase_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetime test value") {
  Data::destrucor_runs = 0;
  {
    data::value_ptr<data::decoration_base<data::has_no_meta::meta>> vp =
        data::make_value<data::decorated_data<Data, data::has_no_meta::meta>>();
    REQUIRE(unerase_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);

  Data::destrucor_runs = 0;
  {
    data::value_ptr<data::decoration_base<data::has_type_info::meta>> vp =
        data::make_value<data::decorated_data<Data, data::has_type_info::meta>>();
    REQUIRE(data::unerase_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);
}
