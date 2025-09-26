// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <cassert>
#include <catch.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/data/has_no_meta/shared_const.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/open_concept/open_concept.hpp>
#include <virtual_void/open_concept/open_visitor.hpp>
#include <virtual_void/utillities/unnamed__.hpp>

using std::cout;
using std::string;

using namespace virtual_void;
using namespace virtual_void::open_concept;

namespace _21_Tree_TE_open_concept {

namespace node {

VV_INTERFACE(node_i, (VV_CONST_METHOD(int, value)))

struct interface;
using model = node_i<data::has_no_meta::shared_const>;
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
static_assert(is_extendable_interface<node::model>);
extension_method<node::model, int(virtual_void::const_)> value;
auto __ = value.define<Plus>(
    [](auto expr) { return value(expr->left) + value(expr->right); });
auto __ = value.define<Times>(
    [](auto expr) { return value(expr->left) * value(expr->right); });
auto __ = value.define<Integer>([](auto expr) { return expr->i; });
//
//-----------------------------------------------------------------------------
// render as Forth
extension_method<node::model, std::string(virtual_void::const_)> as_forth;
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
extension_method<node::model, std::string(virtual_void::const_)> as_lisp;
auto __ = as_lisp.define<Plus>([](auto expr) {
  return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});
auto __ = as_lisp.define<Times>([](auto expr) {
  return "(times " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});
auto __ =
    as_lisp.define<Integer>([](auto expr) { return std::to_string(expr->i); });
//-----------------------------------------------------------------------------
}  // namespace _21_Tree_TE_open_concept

using namespace virtual_void;

VV_RUNTIME_STATIC(type_info, _21_Tree_TE_open_concept::Plus)
VV_RUNTIME_STATIC(type_info, _21_Tree_TE_open_concept::Times)
VV_RUNTIME_STATIC(type_info, _21_Tree_TE_open_concept::Integer)

namespace _21_Tree_TE_open_concept {

TEST_CASE("21_Tree_TE_open_concept") {
  using namespace virtual_void;

  auto expr = node::model{Times{Integer{2}, Plus{Integer{3}, {Integer{4}}}}};

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
  BENCHMARK("21_Tree_TE_open_concept value") { return value(expr); };
  BENCHMARK("21_Tree_TE_open_concept as_lisp") { return as_lisp(expr); };
#endif  // !_DEBUG
}

//-----------------------------------------------------------------------------

////-----------------------------------------------------------------------------
// visiting vistors

// this one must be provided be EACH "node" model:
namespace node {
open_concept::visit<node::model> visit_left_first;
open_concept::visit<node::model> visit_right_first;
open_concept::visit<node::model> visit_center_first;
}  // namespace node

namespace {
auto visit_center = [](auto expr, auto const& visit, auto out, auto in) {
  visit.center(expr, out, in);
};
auto visit_left_right = [](auto expr, auto const& visit, auto out, auto in) {
  visit.head(expr, out, in);
  node::visit_left_first(expr->left, visit, out, in);
  visit.center(expr, out, in);
  node::visit_left_first(expr->right, visit, out, in);
  visit.tail(expr, out, in);
};
auto visit_right_left = [](auto expr, auto const& visit, auto out, auto in) {
  visit.tail(expr, out, in);
  node::visit_right_first(expr->right, visit, out, in);
  visit.center(expr, out, in);
  node::visit_right_first(expr->left, visit, out, in);
  visit.head(expr, out, in);
};
auto visit_center_left_right = [](auto expr, auto const& visit, auto out,
                                  auto in) {
  visit.center(expr, out, in);
  visit.head(expr, out, in);
  node::visit_center_first(expr->left, visit, out, in);
  node::visit_center_first(expr->right, visit, out, in);
  visit.tail(expr, out, in);
};
}  // namespace
auto __ = node::visit_left_first.define<Plus>(visit_left_right);
auto __ = node::visit_left_first.define<Times>(visit_left_right);
auto __ = node::visit_left_first.define<Integer>(visit_center);

auto __ = node::visit_right_first.define<Plus>(visit_right_left);
auto __ = node::visit_right_first.define<Times>(visit_right_left);
auto __ = node::visit_right_first.define<Integer>(visit_center);

auto __ = node::visit_center_first.define<Plus>(visit_center_left_right);
auto __ = node::visit_center_first.define<Times>(visit_center_left_right);
auto __ = node::visit_center_first.define<Integer>(visit_center);

open_concept::visitor<node::model, std::string, char> dump;
auto __ = dump.define_center<Times>(
    [](auto expr, auto& out, auto const& l) { out += "*"; });
auto __ = dump.define_head<Plus>(
    [](auto expr, auto& out, auto const& l) { out += " "; });
auto __ = dump.define_center<Plus>(
    [](auto expr, auto& out, auto const& l) { out += "+"; });
auto __ = dump.define_tail<Plus>(
    [](auto expr, auto& out, auto const& l) { out += " "; });
auto __ = dump.define_center<Integer>([](auto expr, auto& out, auto const&) {
  out += std::to_string(expr->i) + " ";
});

auto pop(std::stack<int>& stack) {
  auto t = stack.top();
  stack.pop();
  return t;
}
open_concept::visitor<node::model, std::stack<int>, char> value_visitor;
auto __ = value_visitor.define_tail<Times>(
    [](auto expr, auto& out, auto const& l) { out.push(pop(out) * pop(out)); });
auto __ =
    value_visitor.define_tail<Plus>([](auto expr, auto& out, auto const& l) {
      out.push(pop(out) + pop(out));
      ;
    });
auto __ = value_visitor.define_center<Integer>(
    [](auto expr, auto& out, auto const&) { out.push(expr->i); });

TEST_CASE("21_Tree_TE_open_concept_with_visitor") {
  using namespace virtual_void;

  auto expr = node::model{Times{Integer{2}, Plus{Integer{3}, {Integer{4}}}}};

  {
    std::stack<int> s;
    value_visitor(expr, node::visit_left_first, s);
    std::cout << s.top() << "\n";
    CHECK(s.top() == 2 * (3 + 4));
  }

  {
    std::string s;
    dump(expr, node::visit_left_first, s);
    std::cout << s << "\n";
    CHECK(s == "2 * 3 +4  ");
  }
  {
    std::string s;
    dump(expr, node::visit_center_first, s);
    std::cout << s << "\n";
    CHECK(s == "*2 + 3 4  ");
  }
  {
    std::string s;
    dump(expr, node::visit_right_first, s);
    std::cout << s << "\n";
    CHECK(s == " 4 +3  *2 ");
  }

#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_open_concept_with_visitor value_visitor") {
    std::stack<int> s;
    value_visitor(expr, node::visit_left_first, s);
  };
  BENCHMARK("21_Tree_TE_open_concept_with_visitor dump") {
    std::string s;
    return dump(expr, node::visit_right_first, s);
  };
#endif  // !_DEBUG
}

}  // namespace _21_Tree_TE_open_concept
