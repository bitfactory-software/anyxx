// virtual_void variant of this yomm2 example via virtual_void m_tables
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <iostream>
#include <string>

#include "../../include/virtual_void/m_table/lifetime/shared_const.h"
#include "../../include/virtual_void/m_table/open_method.h"
#include "../../include/virtual_void/utillities/unnamed__.h"
#include "include/catch.hpp"

using std::cout;
using std::string;

namespace {

using node = virtual_void::m_table::shared_const;

static_assert(virtual_void::erased::is_virtual_void<node>);
static_assert(virtual_void::erased::is_data_pointer<
              virtual_void::m_table::shared_const_data_ptr>);

struct Plus {
  Plus(node left, node right) : left(left), right(right) {}

  node left, right;
  //    ~Plus() { cout << "~Plus()" << "\n"; } // to show, that
  //    virtual_void::typed_shared_const will call the rigtht destructor
};

struct Times {
  Times(node left, node right) : left(left), right(right) {}

  node left, right;
  //    ~Times() { cout << "~Times()" << "\n"; }
};

struct Integer {
  explicit Integer(int value) : value(value) {}
  int value;
  //    ~Integer() { cout << "~Integer()" << "\n"; }
};

// =============================================================================
// add behavior to existing classes, without changing them

virtual_void::m_table::domain tree_domain;

}  // namespace

namespace virtual_void::class_hierarchy {
template <>
struct class_<Plus> : bases<> {};
template <>
struct class_<Times> : bases<> {};
template <>
struct class_<Integer> : bases<> {};

auto __ =
    virtual_void::m_table::declare_classes<Plus, Times, Integer>(tree_domain);
}  // namespace virtual_void::class_hierarchy

namespace {
// -----------------------------------------------------------------------------
// evaluate

auto value = virtual_void::m_table::open_method<int(const void*)>{tree_domain};

auto __ = value.define<Plus>(
    [](auto expr) { return value(expr->left) + value(expr->right); });

auto __ = value.define<Times>(
    [](auto expr) { return value(expr->left) * value(expr->right); });

auto __ = value.define<Integer>([](auto expr) { return expr->value; });

// -----------------------------------------------------------------------------
// render as Forth

auto as_forth =
    virtual_void::m_table::open_method<string(const void*)>{tree_domain};

auto __ = as_forth.define<Plus>([](auto expr) {
  return as_forth(expr->left) + " " + as_forth(expr->right) + " +";
});

auto __ = as_forth.define<Times>([](auto expr) {
  return as_forth(expr->left) + " " + as_forth(expr->right) + " *";
});

auto __ = as_forth.define<Integer>(
    [](auto expr) { return std::to_string(expr->value); });

// -----------------------------------------------------------------------------
// render as Lisp

auto as_lisp =
    virtual_void::m_table::open_method<string(const void*)>{tree_domain};

auto __ = as_lisp.define<Plus>([](auto expr) {
  return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});

auto __ = as_lisp.define<Times>([](auto expr) {
  return "(times " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});

auto __ = as_lisp.define<Integer>(
    [](auto expr) { return std::to_string(expr->value); });

}  // namespace

// -----------------------------------------------------------------------------

TEST_CASE("21_Tree_TE_dispach_via_m_table") {
  virtual_void::m_table::fix_m_tables(tree_domain);

  using namespace virtual_void;

  auto expr = node{Times{{Integer{2}}, {Plus{Integer{3}, Integer{4}}}}};

  REQUIRE(value(expr) == 14);
  std::stringstream out;
  out << as_forth(expr) << " = " << as_lisp(expr) << " = " << value(expr);
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_dispach_via_m_table value") { return value(expr); };
  BENCHMARK("21_Tree_TE_dispach_via_m_table as_lisp") { return as_lisp(expr); };
#endif  // !_DEBUG
}
