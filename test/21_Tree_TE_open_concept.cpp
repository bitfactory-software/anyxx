// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <cassert>
#include <catch.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/data/has_no_meta/shared_const.hpp>
#include <virtual_void/open_concept/open_concept.hpp>
#include <virtual_void/open_concept/open_visitor.hpp>
#include <virtual_void/utillities/unnamed__.hpp>

using std::cout;
using std::string;

using namespace virtual_void;
using namespace virtual_void::open_concept;

namespace _21_Tree_TE_open_concept {

namespace node {
struct interface;
using model =
    virtual_void::open_concept::model<interface,
                                      data::has_no_meta::shared_const>;
};  // namespace node

struct Plus {
  Plus(node::model left, node::model right) : left(left), right(right) {}
  node::model left, right;
};
struct Times {
  Times(node::model left, node::model right) : left(left), right(right) {}
  node::model left, right;
};
struct Integer {
  explicit Integer(int value) : value(value) {}
  int value;
};

// =============================================================================
// add behavior to existing classes, without changing them

//-----------------------------------------------------------------------------
// evaluate
extension_method<node::interface, int(virtual_void::const_)> value;
auto __ = value.define<Plus>(
    [](auto expr) { return value(expr->left) + value(expr->right); });
auto __ = value.define<Times>(
    [](auto expr) { return value(expr->left) * value(expr->right); });
auto __ = value.define<Integer>([](auto expr) { return expr->value; });
//
//-----------------------------------------------------------------------------
// render as Forth
extension_method<node::interface, std::string(virtual_void::const_)> as_forth;
auto __ = as_forth.define<Plus>([](auto expr) {
  return as_forth(expr->left) + " " + as_forth(expr->right) + " +";
});
auto __ = as_forth.define<Times>([](auto expr) {
  return as_forth(expr->left) + " " + as_forth(expr->right) + " *";
});
auto __ = as_forth.define<Integer>(
    [](auto expr) { return std::to_string(expr->value); });
//
//-----------------------------------------------------------------------------
// render as Lisp
extension_method<node::interface, std::string(virtual_void::const_)> as_lisp;
auto __ = as_lisp.define<Plus>([](auto expr) {
  return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});
auto __ = as_lisp.define<Times>([](auto expr) {
  return "(times " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
});
auto __ = as_lisp.define<Integer>(
    [](auto expr) { return std::to_string(expr->value); });
//-----------------------------------------------------------------------------

TEST_CASE("21_Tree_TE_open_concept") {
  using namespace virtual_void;

  open_concept::activate_extension_methods();

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
open_concept::visit<node::interface> visit;
}  // namespace node

namespace {
inline void visit_left_right(auto expr, auto const& visit, auto out, auto in) {
  visit.head(expr, out, in);
  node::visit(expr->left, visit, out, in);
  visit.center(expr, out, in);
  node::visit(expr->right, visit, out, in);
  visit.tail(expr, out, in);
}
}  // namespace
auto __ = node::visit.define<Plus>(
    [](auto expr, auto const& visit, auto out, auto in) {
      visit_left_right(expr, visit, out, in);
    });
auto __ = node::visit.define<Times>(
    [](auto expr, auto const& visit, auto out, auto in) {
      visit_left_right(expr, visit, out, in);
    });
auto __ = node::visit.define<Integer>(
    [](Integer const* expr, auto const& visit, auto out, auto in) {
      visit.center(expr, out, in);
    });

open_concept::typed_visitor<node::interface, std::string, nullptr_t> dump;
auto __ = dump.define_center<Integer>(
    [](Integer const* expr, std::string& out, nullptr_t const&) {
      out += std::to_string(expr->value) + ";";
    });

// node::visitor dump_as_lisp;
// auto __ = dump_as_lisp.head.define<Plus>([](auto expr) {
//   return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
// });
// auto __ = dump_as_lisp.center.define<Plus>([](auto expr) {
//   return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
// });
// auto __ = dump_as_lisp.center.define<Plus>([](auto expr) {
//   return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
// });
// auto __ = dump_as_lisp.center.define<Plus>([](auto expr) {
//   return "(plus " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
// });
// auto __ = dump_as_lisp.define<Times>([](auto expr) {
//   return "(times " + as_lisp(expr->left) + " " + as_lisp(expr->right) + ")";
// });
// auto __ = dump_as_lisp.center.define<Integer>(
//     [](Integer const* expr, auto out, auto)
//     {
//       auto s = unerase_cast<std::string*>(out);
//       (*s) += std::to_string(expr->value);
//     });

TEST_CASE("21_Tree_TE_open_concept_with_visitor") {
  using namespace virtual_void;

  open_concept::activate_extension_methods();

  auto expr = node::model{Times{Integer{2}, Plus{Integer{3}, {Integer{4}}}}};

  std::string s;
  dump(expr, node::visit, s);
  std::cout << s << "\n";
  REQUIRE(s == "2;3;4;");

#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_open_concept_with_visitor dump") {
    // std::stringstream outstream;
    // node::any out(&outstream);
    std::string s;
    return node::visit(expr, dump, &s, nullptr);
  };
#endif  // !_DEBUG
}

}  // namespace _21_Tree_TE_open_concept
