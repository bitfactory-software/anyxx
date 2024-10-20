#include <any>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../include/virtual_void/erased/lifetime/data.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

namespace virtual_void::erased {

struct empty_meta_t {
  void* data();
  void const* data() const;
};

template <typename T, typename M = empty_meta_t>
struct data_aligned : M {
  using meta_t = M;
  T t;
};

void* empty_meta_t::data() {
  return &static_cast<data_aligned<int, empty_meta_t>*>(this)->t;
};
void const* empty_meta_t::data() const {
  return &static_cast<data_aligned<int, empty_meta_t> const*>(this)->t;
};

template <typename M>
struct ritch_meta_t {
  M m;
  void* data() {
    return &static_cast<data_aligned<int, ritch_meta_t<M>>*>(this)->t;
  }
  void const* data() const {
    return &static_cast<data_aligned<int, ritch_meta_t<M>> const*>(this)->t;
  }
};

template <typename TO, typename DATA>
const TO* unerase_data_cast(const DATA& data) {
  return static_cast<TO*>(data.data());
}
template <typename TO, typename DATA>
TO* unerase_data_cast(DATA& data) {
  return static_cast<TO*>(data.data());
}

template <typename M>
using data_deleter = void (*)(M*);
template <typename M>
using unique_data_ptr = std::unique_ptr<M, data_deleter<M>>;
template <typename T, typename... ARGS>
auto make_unique_data_ptr(ARGS&&... args) {
  using meta_t = T::meta_t;
  auto deleter = +[](meta_t* meta) { delete static_cast<T*>(meta); };
  return unique_data_ptr<meta_t>(new T(std::forward<ARGS>(args)...), deleter);
}

template <typename M>
struct value_v_table_t {
  using destroy_fn = void(M*) noexcept;
  using copy_fn = M*(const M*);
  template <class T>
  constexpr value_v_table_t(std::in_place_type_t<T>)
      : destroy(&destroy_impl<T>), copy(&copy_impl<T>) {}
  template <class T>
  static void destroy_impl(M* target) noexcept {
    ::delete static_cast<T*>(target);
  }
  template <class T>
  static M* copy_impl(M const* source) {
    return ::new T(*static_cast<T const*>(source));
  }
  destroy_fn* destroy;
  copy_fn* copy;
};

template <class T>
constexpr value_v_table_t<typename T::meta_t> value_v_table =
    value_v_table_t<typename T::meta_t>(std::in_place_type<T>);

template <typename M>
class value_ptr {
 public:
  template <typename DATA>
  value_ptr(DATA* v) : m_(v), v_table_(&value_v_table<std::decay_t<DATA>>) {}
  value_ptr(value_ptr const& rhs)
      : m_(rhs.v_table_->copy(rhs.m_)), v_table_(rhs.v_table_) {}
  ~value_ptr() { v_table_->destroy(m_); }
  M* meta() { return m_; }
  M const* meta() const { return m_; }
  M& operator*() { return *meta(); }
  const M& operator*() const { return *meta(); }
  M* operator->() { return meta(); }
  const M* operator->() const { return meta(); }

 private:
  M* m_;
  const value_v_table_t<M>* v_table_;
};

template <typename T, typename... ARGS>
auto make_value_data_ptr(ARGS&&... args) {
  using meta_t = T::meta_t;
  auto deleter = +[](meta_t* meta) { delete static_cast<T*>(meta); };
  return value_ptr<meta_t>(new T(std::forward<ARGS>(args)...));
}

}  // namespace virtual_void::erased
using namespace virtual_void;
using namespace virtual_void::erased;

#define DATA_ALIGNED(T, M) data_aligned<T, ritch_meta_t<M>>

#define ASSERT_OFFSET_EMPTY(T, o) \
  static_assert(offsetof(data_aligned<T>, t) == o);

#define ASSERT_OFFSET(T, M, o) \
  static_assert(offsetof(DATA_ALIGNED(T, M), t) == o);

ASSERT_OFFSET_EMPTY(char, 0);
ASSERT_OFFSET_EMPTY(int, 0);
ASSERT_OFFSET_EMPTY(char const*, 0)
ASSERT_OFFSET_EMPTY(std::string, 0)

ASSERT_OFFSET(char, std::type_info const*, 8);
ASSERT_OFFSET(int, std::type_info const*, 8);
ASSERT_OFFSET(char const*, std::type_info const*, 8);
ASSERT_OFFSET(std::string, std::type_info const*, 8);

#define TRACE_OFFSET_EMPTY(T)                                         \
  {                                                                   \
    using TYPE = data_aligned<T>;                                     \
    std::cout << "data_aligned<" << #T                                \
              << "> offsetof(t): " << offsetof(data_aligned<TYPE>, t) \
              << std::endl;                                           \
  }

#define TRACE_OFFSET(T, M)                                            \
  {                                                                   \
    using TYPE = data_aligned<T, ritch_meta_t<M>>;                    \
    std::cout << "data_aligned<" << #T << ", " << #M                  \
              << "> offsetof(t): " << offsetof(TYPE, t)               \
              << ", offsetof(m): " << offsetof(TYPE, m) << std::endl; \
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
    auto unique_data_ptr = erased::make_unique_data_ptr<data_aligned<Data>>();
    REQUIRE(unerase_data_cast<Data>(*unique_data_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    auto unique_data_ptr = erased::make_unique_data_ptr<
        data_aligned<Data, ritch_meta_t<const std::type_info*>>>();
    REQUIRE(unerase_data_cast<Data>(*unique_data_ptr)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetiem test shared") {
  Data::destrucor_runs = 0;
  {
    std::shared_ptr<empty_meta_t> sp = std::make_shared<data_aligned<Data>>();
    REQUIRE(unerase_data_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    std::shared_ptr<ritch_meta_t<const std::type_info*>> sp = std::make_shared<
        data_aligned<Data, ritch_meta_t<const std::type_info*>>>();
    REQUIRE(unerase_data_cast<Data>(*sp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
TEST_CASE("erase lifetiem test value") {
  Data::destrucor_runs = 0;
  {
    value_ptr<empty_meta_t> vp = make_value_data_ptr<data_aligned<Data>>();
    REQUIRE(unerase_data_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);

  Data::destrucor_runs = 0;
  {
    value_ptr<ritch_meta_t<const std::type_info*>> vp = make_value_data_ptr<data_aligned< Data, ritch_meta_t<const std::type_info*>>>();
    REQUIRE(unerase_data_cast<Data>(*vp)->s_ == "hello world");
    REQUIRE(Data::destrucor_runs == 0);
    auto vp2 = vp;
  }
  REQUIRE(Data::destrucor_runs == 2);
  // std::any  any_string{ std::string("hello world")};
  // auto y = any_string;
}
