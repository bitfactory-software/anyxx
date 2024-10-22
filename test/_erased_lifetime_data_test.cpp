#include <any>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/data/has_no_meta/has_no_meta.h"
#include "../include/virtual_void/erased/data/holder.h"
#include "../include/virtual_void/erased/data/shared_const_ptr.h"
#include "../include/virtual_void/erased/data/unique_ptr.h"
#include "../include/virtual_void/erased/data/value_ptr.h"
#include "../include/virtual_void/erased/virtual_void.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace virtual_void;
using namespace virtual_void::erased;

namespace virtual_void::erased::data {
template <typename META_DATA>  // obsolate! replace with base<>!
struct with_meta {
  META_DATA meta_data_;
  template <typename T>
  with_meta(std::in_place_type_t<T>) : meta_data_(std::in_place_type<T>) {}
  void* data() {
    return &static_cast<holder<int, with_meta<META_DATA>>*>(this)->value_;
  }
  void const* data() const {
    return &static_cast<holder<int, with_meta<META_DATA>> const*>(this)
                ->the_data_;
  }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~with_meta() = default;
};
}  // namespace virtual_void::erased::data

#define DATA_ALIGNED(T, META_DATA) data::holder<T, META_DATA>

#define ASSERT_OFFSET_EMPTY(T, o) \
  static_assert(offsetof(DATA_ALIGNED(T, data::has_no_meta), value_) == o);

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

ASSERT_OFFSET(char, data::has_type_info, 8 + offset_for_v_table);
ASSERT_OFFSET(int, data::has_type_info, 8 + offset_for_v_table);
ASSERT_OFFSET(char const*, data::has_type_info, 8 + offset_for_v_table);
ASSERT_OFFSET(std::string, data::has_type_info, 8 + offset_for_v_table);

#define TRACE_OFFSET_EMPTY(T)                                     \
  {                                                               \
    using TYPE = data::holder<T, data::has_no_meta>;              \
    std::cout << "data::holder<" << #T                            \
              << "> offsetof(value_): " << offsetof(TYPE, value_) \
              << std::endl;                                       \
  }

#define TRACE_OFFSET(T, META_DATA)                                \
  {                                                               \
    using TYPE = data::holder<T, META_DATA>;                      \
    std::cout << "data::holder<" << #T << ", " << #META_DATA      \
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

TEST_CASE("erase lifetiem test") {
  TRACE_OFFSET_EMPTY(char);
  TRACE_OFFSET_EMPTY(int);
  TRACE_OFFSET_EMPTY(char const*);
  TRACE_OFFSET_EMPTY(std::string);

  TRACE_OFFSET(char, data::has_type_info);
  TRACE_OFFSET(int, data::has_type_info);
  TRACE_OFFSET(char const*, data::has_type_info);
  TRACE_OFFSET(std::string, data::has_type_info);
}
TEST_CASE("erase lifetiem test unique") {
  Data::destrucor_runs = 0;
  {
    auto unique_ptr =
        erased::data::make_unique<data::holder<Data, data::has_no_meta>>();
    REQUIRE(unerase_cast<Data>(*unique_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    auto unique_ptr =
        erased::data::make_unique<data::holder<Data, data::has_type_info>>();
    REQUIRE(unerase_cast<Data>(*unique_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetiem test shared") {
  Data::destrucor_runs = 0;
  {
    std::shared_ptr<data::with_no_meta const> sp =
        data::make_shared_const<data::holder<Data, data::has_no_meta>>();
    REQUIRE(data::unerase_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    std::shared_ptr<data::with_type_info const> sp = data::make_shared_const<
        data::holder<Data, data::has_type_info> const>();
    REQUIRE(data::unerase_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetiem test value") {
  Data::destrucor_runs = 0;
  {
    data::value_ptr<data::with_no_meta> vp =
        data::make_value<data::holder<Data, data::has_no_meta>>();
    REQUIRE(unerase_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);

  Data::destrucor_runs = 0;
  {
    data::value_ptr<data::with_type_info> vp =
        data::make_value<data::holder<Data, data::has_type_info>>();
    REQUIRE(data::unerase_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);
}
