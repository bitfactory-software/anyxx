#include <any>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/data/typed.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

namespace virtual_void::erased {


template <typename META_DATA>
using data_deleter = void (*)(META_DATA*);
template <typename META_DATA>
using unique_data_ptr = std::unique_ptr<META_DATA, data_deleter<META_DATA>>;
template <typename T, typename... ARGS>
auto make_unique_data_ptr(ARGS&&... args) {
  using meta_data_t = T::meta_data_t;
  auto deleter = +[](meta_data_t* meta) { delete static_cast<T*>(meta); };
  return unique_data_ptr<meta_data_t>(
      new T(std::in_place, std::forward<ARGS>(args)...), deleter);
}

template <typename META_DATA>
struct value_v_table_t {
  using destroy_fn = void(META_DATA*) noexcept;
  using copy_fn = META_DATA*(const META_DATA*);
  template <class T>
  constexpr value_v_table_t(std::in_place_type_t<T>)
      : destroy(&destroy_impl<T>), copy(&copy_impl<T>) {}
  template <class T>
  static void destroy_impl(META_DATA* target) noexcept {
    ::delete static_cast<T*>(target);
  }
  template <class T>
  static META_DATA* copy_impl(META_DATA const* source) {
    return ::new T(*static_cast<T const*>(source));
  }
  destroy_fn* destroy;
  copy_fn* copy;
};

template <class T>
constexpr value_v_table_t<typename T::meta_data_t> value_v_table =
    value_v_table_t<typename T::meta_data_t>(std::in_place_type<T>);

template <typename META_DATA>
class value_ptr {
 public:
  template <typename DATA>
  value_ptr(DATA* v) : m_(v), v_table_(&value_v_table<std::decay_t<DATA>>) {}
  value_ptr(value_ptr const& rhs)
      : m_(rhs.v_table_->copy(rhs.m_)), v_table_(rhs.v_table_) {}
  ~value_ptr() { v_table_->destroy(m_); }
  META_DATA* meta() { return m_; }
  META_DATA const* meta() const { return m_; }
  META_DATA& operator*() { return *meta(); }
  const META_DATA& operator*() const { return *meta(); }
  META_DATA* operator->() { return meta(); }
  const META_DATA* operator->() const { return meta(); }

 private:
  META_DATA* m_;
  const value_v_table_t<META_DATA>* v_table_;
};

template <typename T, typename... ARGS>
auto make_value_data_ptr(ARGS&&... args) {
  using meta_data_t = T::meta_data_t;
  auto deleter = +[](meta_data_t* meta) { delete static_cast<T*>(meta); };
  return value_ptr<meta_data_t>(
      new T(std::in_place, std::forward<ARGS>(args)...));
}

}  // namespace virtual_void::erased
using namespace virtual_void;
using namespace virtual_void::erased;

#define DATA_ALIGNED(T, META_DATA) typed_data<T, ritch_meta_data<META_DATA>>

#define ASSERT_OFFSET_EMPTY(T, o) \
  static_assert(offsetof(typed_data<T>, the_data_) == o);

#define ASSERT_OFFSET(T, META_DATA, o) \
  static_assert(offsetof(DATA_ALIGNED(T, META_DATA), the_data_) == o);

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

ASSERT_OFFSET(char, std::type_info const*, 8 + offset_for_v_table);
ASSERT_OFFSET(int, std::type_info const*, 8 + offset_for_v_table);
ASSERT_OFFSET(char const*, std::type_info const*, 8 + offset_for_v_table);
ASSERT_OFFSET(std::string, std::type_info const*, 8 + offset_for_v_table);

#define TRACE_OFFSET_EMPTY(T)                                        \
  {                                                                  \
    using TYPE = typed_data<T>;                                      \
    std::cout << "typed_data<" << #T << "> offsetof(the_data_): "    \
              << offsetof(typed_data<TYPE>, the_data_) << std::endl; \
  }

#define TRACE_OFFSET(T, META_DATA)                                        \
  {                                                                       \
    using TYPE = typed_data<T, ritch_meta_data<META_DATA>>;                  \
    std::cout << "typed_data<" << #T << ", " << #META_DATA                \
              << "> offsetof(the_data_): " << offsetof(TYPE, the_data_)   \
              << ", offsetof(meta_data_): " << offsetof(TYPE, meta_data_) \
              << std::endl;                                               \
  }

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

  TRACE_OFFSET(char, std::type_info const*);
  TRACE_OFFSET(int, std::type_info const*);
  TRACE_OFFSET(char const*, std::type_info const*);
  TRACE_OFFSET(std::string, std::type_info const*);
}
TEST_CASE("erase lifetiem test unique") {
  Data::destrucor_runs = 0;
  {
    auto unique_data_ptr = erased::make_unique_data_ptr<typed_data<Data>>();
    REQUIRE(unerase_data_cast<Data>(*unique_data_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    auto unique_data_ptr =
        erased::make_unique_data_ptr<typed_data<Data, type_info_meta_data>>();
    REQUIRE(unerase_data_cast<Data>(*unique_data_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetiem test shared") {
  Data::destrucor_runs = 0;
  {
    std::shared_ptr<no_meta const> sp =
        std::make_shared<typed_data<Data>>(std::in_place);
    REQUIRE(unerase_data_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    std::shared_ptr<type_info_meta_data const> sp =
        std::make_shared<typed_data<Data, type_info_meta_data> const>(
            std::in_place);
    REQUIRE(unerase_data_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetiem test value") {
  Data::destrucor_runs = 0;
  {
    value_ptr<no_meta> vp = make_value_data_ptr<typed_data<Data>>();
    REQUIRE(unerase_data_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);

  Data::destrucor_runs = 0;
  {
    value_ptr<type_info_meta_data> vp =
        make_value_data_ptr<typed_data<Data, type_info_meta_data>>();
    REQUIRE(unerase_data_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);
  // std::any  any_string{ std::string("hello world")};
  // auto y = any_string;
}
