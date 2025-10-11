// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <cassert>
#include <catch.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <virtual_void/data/observer.hpp>
#include <virtual_void/data/shared_const.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/extension_method.hpp>
#include <virtual_void/utillities/unnamed__.hpp>

using std::cout;
using std::string;

using namespace virtual_void;
using namespace virtual_void::interface;

VV_V_TABLE_HAS_EXTENSION_METHODS(_21_Tree_TE_interface_extension_method::node,
                                 node_i)

namespace _21_Tree_TE_interface_extension_method {

namespace node {
VV_INTERFACE(node_i, (VV_CONST_METHOD(int, value)))
}
}  // namespace _21_Tree_TE_interface_extension_method

VV_EXTENSION_METHOD_COUNT(_21_Tree_TE_interface_extension_method::node::node_i)

namespace _21_Tree_TE_interface_extension_method {
namespace node {
struct interface;
using model = node_i<data::shared_const>;
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

}  // namespace _21_Tree_TE_interface_extension_method

VV_EXTENSION_TABLE_INSTANCE(
    _21_Tree_TE_interface_extension_method::Plus,
    _21_Tree_TE_interface_extension_method::node::node_i)
VV_EXTENSION_TABLE_INSTANCE(
    _21_Tree_TE_interface_extension_method::Times,
    _21_Tree_TE_interface_extension_method::node::node_i)
VV_EXTENSION_TABLE_INSTANCE(
    _21_Tree_TE_interface_extension_method::Integer,
    _21_Tree_TE_interface_extension_method::node::node_i)

namespace _21_Tree_TE_interface_extension_method {

// =============================================================================
// add behavior to existing classes, without changing them

//-----------------------------------------------------------------------------
// evaluate
extension_method<int(virtual_<node::model>)> value;
auto __ = value.define<Plus>(
    [](auto expr) { return value(expr->left) + value(expr->right); });
auto __ = value.define<Times>(
    [](auto expr) { return value(expr->left) * value(expr->right); });
auto __ = value.define<Integer>([](auto expr) { return expr->i; });
//
//-----------------------------------------------------------------------------
// render as Forth
extension_method<std::string(virtual_<node::model>)> as_forth;
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
extension_method<std::string(virtual_<node::model>)> as_lisp;
auto __ = as_lisp.define<Plus>([](auto expr) {
  return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});
auto __ = as_lisp.define<Times>([](auto expr) {
  return "(times " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});
auto __ =
    as_lisp.define<Integer>([](auto expr) { return std::to_string(expr->i); });
//-----------------------------------------------------------------------------
}  // namespace _21_Tree_TE_interface_extension_method

using namespace virtual_void;

namespace _21_Tree_TE_interface_extension_method {

TEST_CASE("21_Tree_TE_interface_extension_method") {
  using namespace virtual_void;

  auto expr = node::model{std::make_shared<Times>(
      std::make_shared<Integer>(2),
      std::make_shared<Plus>(std::make_shared<Integer>(3),
                             std::make_shared<Integer>(4)))};

  // REQUIRE(&v_table_instance<node::interface, Times>() == &get_v_table(expr));
  // REQUIRE(v_table_instance<node::interface, Times>().size() >= 3u);
  // REQUIRE(v_table_instance<node::interface, Times>()[0]);
  // REQUIRE(v_table_instance<node::interface, Times>()[1]);
  // REQUIRE(v_table_instance<node::interface, Times>()[2]);

  auto v = value(expr);
  REQUIRE(v == 14);
  std::stringstream out;
  out << as_forth(expr) << " = " << as_lisp(expr) << " = " << value(expr);
  std::cout << out.str() << "\n";
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");

#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_interface_extension_method value") {
    return value(expr);
  };
  BENCHMARK("21_Tree_TE_interface_extension_method as_lisp") {
    return as_lisp(expr);
  };
#endif  // !_DEBUG
}

}  // namespace _21_Tree_TE_interface_extension_method