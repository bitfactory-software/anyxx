// virtual_void variant of this yomm2 example via virtual_void m_tables
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <any>
#include <iostream>
#include <string>

#include "../../include/virtual_void/any_dispatch/method_typeid_hash.h"
#include "../../include/virtual_void/utillities/unnamed__.h"
#include "include/catch.hpp"

using std::cout;
using std::string;

namespace {

using any = std::any;

struct Plus {
  Plus(any left, any right) : left(left), right(right) {}

  any left, right;
};

struct Times {
  Times(any left, any right) : left(left), right(right) {}

  any left, right;
};

struct Integer {
  explicit Integer(int value) : value(value) {}
  int value;
};

// =============================================================================
// add behavior to existing classes, without changing them

// -----------------------------------------------------------------------------
// evaluate

auto value = any_dispatch::method_typeid_hash<int, const any&>{};

auto __ = value.define<Plus>(
    [](auto expr) { return value(expr->left) + value(expr->right); });

auto __ = value.define<Times>(
    [](auto expr) { return value(expr->left) * value(expr->right); });

auto __ = value.define<Integer>([](auto expr) { return expr->value; });

// -----------------------------------------------------------------------------
// render as Forth

auto as_forth = any_dispatch::method_typeid_hash<string, const any&>{};

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

auto as_lisp = any_dispatch::method_typeid_hash<string, const any&>{};

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

TEST_CASE("21_Tree_TE_dispatch_via_any_and_type_index") {
  value.seal();
  as_forth.seal();
  as_lisp.seal();

  using std::make_any;

  auto expr = make_any<Times>(
      make_any<Integer>(2),
      make_any<Plus>(make_any<Integer>(3), make_any<Integer>(4)));

  REQUIRE(value(expr) == 14);
  std::stringstream out;
  out << as_forth(expr) << " = " << as_lisp(expr) << " = " << value(expr);
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_dispatch_via_any_and_type_index value") {
    return value(expr);
  };
  BENCHMARK("21_Tree_TE_dispatch_via_any_and_type_index as_lisp") {
    return as_lisp(expr);
  };
#endif  // !_DEBUG
}
