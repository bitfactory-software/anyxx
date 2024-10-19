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
};

template <typename T, typename M = empty_meta_t>
struct data_aligned : M {
  using meta_t = M;
  T t;
};

void* empty_meta_t::data() {
  return &static_cast<data_aligned<int, empty_meta_t>*>(this)->t;
};

template <typename M>
struct meta_t {
  M m;
  void* data() { return &static_cast<data_aligned<int, meta_t<M>>*>(this)->t; }
};

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

}  // namespace virtual_void::erased
using namespace virtual_void;
using namespace virtual_void::erased;

#define DATA_ALIGNED(T, M) data_aligned<T, meta_t<M>>

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
    using TYPE = data_aligned<T, meta_t<M>>;                          \
    std::cout << "data_aligned<" << #T << ", " << #M                  \
              << "> offsetof(t): " << offsetof(TYPE, t)               \
              << ", offsetof(m): " << offsetof(TYPE, m) << std::endl; \
  }

struct Data {
  static int destrucor_runs;
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
  TRACE_OFFSET(std::string, std::type_info const*);

  Data::destrucor_runs = 0;
  {
    auto unique_data_ptr = erased::make_unique_data_ptr<data_aligned<Data>>();
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);

  Data::destrucor_runs = 0;
  {
    std::shared_ptr<empty_meta_t> sp = std::make_shared<data_aligned<Data>>();
    REQUIRE(Data::destrucor_runs == 0);
  }
  REQUIRE(Data::destrucor_runs == 1);
}
