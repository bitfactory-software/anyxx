﻿// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <anyxx/anyxx.hpp>
#include <catch.hpp>
#include <iostream>
#include <string>

using std::cout;
using std::string;

using namespace anyxx;
using namespace anyxx;

ANY_HAS_DISPATCH(_21_Tree_TE_interface_dispatch::node, node_i)

namespace _21_Tree_TE_interface_dispatch {

namespace node {
ANY(node_i, (ANY_CONST_METHOD(int, value)))
}
}  // namespace _21_Tree_TE_interface_dispatch

namespace _21_Tree_TE_interface_dispatch {
namespace node {
struct any;
using model = node_i<shared_const>;
};  // namespace node

struct Plus {
  Plus(node::model left, node::model right) : left(left), right(right) {}
  node::model left, right;
  int value() const { return left.value() + right.value(); }
};
struct Times {
  Times(node::model left, node::model right) : left(left), right(right) {}
  node::model left, right;
  int value() const { return left.value() * right.value(); }
};
struct Integer {
  explicit Integer(int i_) : i(i_) {}
  int i;
  int value() const { return i; }
};

// =============================================================================
// add behavior to existing classes, without changing them

//-----------------------------------------------------------------------------
// evaluate
method<int(virtual_<node::model>)> value;
auto __ = value.define<Plus>(
    [](auto expr) { return value(expr->left) + value(expr->right); });
auto __ = value.define<Times>(
    [](auto expr) { return value(expr->left) * value(expr->right); });
auto __ = value.define<Integer>([](auto expr) { return expr->i; });
//
//-----------------------------------------------------------------------------
// render as Forth
method<std::string(virtual_<node::model>)> as_forth;
auto __ = as_forth.define<Plus>([](auto expr) {
  return as_forth(expr->left) + " " + as_forth(expr->right) + " +";
});
auto __ = as_forth.define<Times>([](auto expr) {
  return as_forth(expr->left) + " " + as_forth(expr->right) + " *";
});
auto __ =
    as_forth.define<Integer>([](auto expr) { return std::to_string(expr->i); });
//
//-----------------------------------------------------------------------------
// render as Lisp
method<std::string(virtual_<node::model>)> as_lisp;
auto __ = as_lisp.define<Plus>([](auto expr) {
  return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});
auto __ = as_lisp.define<Times>([](auto expr) {
  return "(times " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});
auto __ =
    as_lisp.define<Integer>([](auto expr) { return std::to_string(expr->i); });
//-----------------------------------------------------------------------------
}  // namespace _21_Tree_TE_interface_dispatch

using namespace anyxx;

namespace _21_Tree_TE_interface_dispatch {

TEST_CASE("21_Tree_TE_interface_dispatch") {
  using namespace anyxx;

  auto expr = node::model{std::make_shared<Times>(
      std::make_shared<Integer>(2),
      std::make_shared<Plus>(std::make_shared<Integer>(3),
                             std::make_shared<Integer>(4)))};

  // REQUIRE(&v_table_instance<node, Times>() == &get_v_table(expr));
  // REQUIRE(v_table_instance<node, Times>().size() >= 3u);
  // REQUIRE(v_table_instance<node, Times>()[0]);
  // REQUIRE(v_table_instance<node, Times>()[1]);
  // REQUIRE(v_table_instance<node, Times>()[2]);

  auto v = value(expr);
  REQUIRE(v == 14);
  std::stringstream out;
  out << as_forth(expr) << " = " << as_lisp(expr) << " = " << value(expr);
  std::cout << out.str() << "\n";
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");

#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_interface_dispatch value") { return value(expr); };
  BENCHMARK("21_Tree_TE_interface_dispatch as_lisp") { return as_lisp(expr); };
#endif  // !_DEBUG
}

}  // namespace _21_Tree_TE_interface_dispatch