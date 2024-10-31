// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <iostream>
#include <memory>
#include <string>

#include "virtual_void/data/shared_const_ptr.h"
#include "virtual_void/interface/declare_macro.h"
#include "virtual_void/data/has_no_meta/shared_const.h"
#include "include/catch.hpp"

using std::cout;
using std::string;

namespace {

ERASED_INTERFACE(node_i, (INTERFACE_CONST_METHOD(int, value_),
                          INTERFACE_CONST_METHOD(string, as_forth),
                          INTERFACE_CONST_METHOD(string, as_lisp)))

using node = node_i<virtual_void::data::has_no_meta::shared_const>;

struct Plus {
  Plus(node left, node right) : left(left), right(right) {}
  int value_() const { return left.value_() + right.value_(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " +";
  }
  string as_lisp() const {
    return "(plus " + left.as_lisp() + " " + right.as_lisp() + ")";
  }

  node left, right;
};

struct Times {
  Times(node left, node right) : left(left), right(right) {}
  int value_() const { return left.value_() * right.value_(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " *";
  }
  string as_lisp() const {
    return "(times " + left.as_lisp() + " " + right.as_lisp() + ")";
  }

  node left, right;
};

struct Integer {
  explicit Integer(int value) : int_(value) {}
  int value_() const { return int_; }
  string as_forth() const { return std::to_string(int_); }
  string as_lisp() const { return std::to_string(int_); }

  int int_;
};

template <typename NODE, typename... ARGS>
auto make_node(ARGS&&... args) {
  return virtual_void::data::has_no_meta::typed_shared_const<NODE>{std::in_place,
      std::forward<ARGS>(args)...};
}

}  // namespace

TEST_CASE("21_Tree_TE_dynamic_interface") {
  using namespace virtual_void;

  static_assert(is_virtual_typed<data::has_no_meta::typed_shared_const<Times>>);
  static_assert(is_virtual_typed<data::has_no_meta::typed_shared_const<Plus>>);
  static_assert(is_virtual_typed<data::has_no_meta::typed_shared_const<Integer>>);

  auto expr = node(make_node<Times>(
      make_node<Integer>(2),
      make_node<Plus>(make_node<Integer>(3), make_node<Integer>(4))));

  REQUIRE(expr.value_() == 14);
  std::stringstream out;
  out << expr.as_forth() << " = " << expr.as_lisp() << " = " << expr.value_();
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_dynamic_interface value") { return expr.value(); };
  BENCHMARK("21_Tree_TE_dynamic_interface as_lisp") { return expr.as_lisp(); };
#endif  // !_DEBUG
}
