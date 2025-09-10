// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <any>
#include <cassert>
#include <catch.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <virtual_void/data/has_no_meta/observer.hpp>
#include <virtual_void/data/has_no_meta/shared_const.hpp>
#include <virtual_void/open_concept/open_concept.hpp>
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

  //REQUIRE(&v_table_instance<node::interface, Times>() == &get_v_table(expr));
  //REQUIRE(v_table_instance<node::interface, Times>().size() >= 3u);
  //REQUIRE(v_table_instance<node::interface, Times>()[0]);
  //REQUIRE(v_table_instance<node::interface, Times>()[1]);
  //REQUIRE(v_table_instance<node::interface, Times>()[2]);

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
//namespace node {
//struct visitor {
//  struct interface;
//  using model =
//      virtual_void::open_concept::model<interface,
//                                        data::has_no_meta::const_observer>;
//  using method_t =
//      extension_method<interface,
//                       void(virtual_void::const_, std::any*, std::any const&)>;
//  method_t head, center, tail;
//};
//extension_method<node::interface, void(virtual_void::const_, visitor const&,
//                                       std::any*, std::any const&)>
//    visit;
//}  // namespace node
//namespace {
//inline void visit_left_right(auto const& expr, node::visitor const& visit,
//                             std::any* out, std::any const& in) {
//  node::visitor::model m{*expr};
//  visit.head(m, out, in);
//  node::visit(expr->left, visit, out, in);
//  visit.center(m, out, in);
//  node::visit(expr->right, visit, out, in);
//  visit.tail(m, out, in);
//}
//}  // namespace
//auto __ = node::visit.define<Plus>(
//    [](auto expr, node::visitor const& visit, std::any* out,
//       std::any const& in) { visit_left_right(expr, visit, out, in); });
//auto __ = node::visit.define<Times>(
//    [](auto expr, node::visitor const& visit, std::any* out,
//       std::any const& in) { visit_left_right(expr, visit, out, in); });
//auto __ = node::visit.define<Integer>([](Integer const* expr,
//                                         node::visitor const& visit,
//                                         std::any* out, std::any const& in) {
//  visit.center(node::visitor::model{*expr}, out, in);
//});
//
//TEST_CASE("21_Tree_TE_open_concept_with_visitor") {
//  using namespace virtual_void;
//
//  open_concept::activate_extension_methods();
//
//  auto expr = node::model{Times{Integer{2}, Plus{Integer{3}, {Integer{4}}}}};
//
//    auto v = value(expr);
//    REQUIRE(v == 14);
//    std::stringstream out;
//    out << as_forth(expr) << " = " << as_lisp(expr) << " = " << value(expr);
//    std::cout << out.str() << "\n";
//    REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
//  
//  // #ifndef _DEBUG
//  //  BENCHMARK("21_Tree_TE_open_concept value") { return value(expr); };
//  //  BENCHMARK("21_Tree_TE_open_concept as_lisp") { return as_lisp(expr); };
//  // #endif  // !_DEBUG
//}

}  // namespace _21_Tree_TE_open_concept
