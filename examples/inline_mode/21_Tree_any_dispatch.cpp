// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <bit_factory/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

using namespace anyxx;

namespace _21_Tree_TE_interface_dispatch {

struct node_has_open_dispatch {};
ANY(node, , shared) // val)

struct Plus {
  Plus(any_node<> left, any_node<> right)
      : left_(std::move(left)), right_(std::move(right)) {}
  any_node<> left_, right_;
};
struct Times {
  Times(any_node<> left, any_node<> right)
      : left_(std::move(left)), right_(std::move(right)) {}
  any_node<> left_, right_;
};
struct Integer {
  explicit Integer(int i_) : i(i_) {}
  int i;
};

// =============================================================================
// add behavior to existing classes, without changing them

//-----------------------------------------------------------------------------
// open dispatch evaluate, returns int, dispatched via an any_node;
dispatch<int(virtual_<any_node<>>)> evaluate;
auto __ = evaluate.define<Plus>([](auto const& expr) {
  return evaluate(expr.left_) + evaluate(expr.right_);
});
auto __ = evaluate.define<Times>([](auto const& expr) {
  return evaluate(expr.left_) * evaluate(expr.right_);
});
auto __ = evaluate.define<Integer>([](auto const& expr) { return expr.i; });
//
//-----------------------------------------------------------------------------
// render as Forth
dispatch<std::string(virtual_<any_node<>>)> as_forth;
auto __ = as_forth.define<Plus>([](auto const& expr) {
  return as_forth(expr.left_) + " " + as_forth(expr.right_) + " +";
});
auto __ = as_forth.define<Times>([](auto const& expr) {
  return as_forth(expr.left_) + " " + as_forth(expr.right_) + " *";
});
auto __ = as_forth.define<Integer>(
    [](auto const& expr) { return std::to_string(expr.i); });
//
//-----------------------------------------------------------------------------
// render as Lisp
dispatch<std::string(virtual_<any_node<>>)> as_lisp;
auto __ = as_lisp.define<Plus>([](auto const& expr) {
  return "(plus " + as_lisp(expr.left_) + " " + as_lisp(expr.right_) + ")";
});
auto __ = as_lisp.define<Times>([](auto const& expr) {
  return "(times " + as_lisp(expr.left_) + " " + as_lisp(expr.right_) + ")";
});
auto __ = as_lisp.define<Integer>(
    [](auto const& expr) { return std::to_string(expr.i); });
//-----------------------------------------------------------------------------
}  // namespace _21_Tree_TE_interface_dispatch

using namespace anyxx;

namespace _21_Tree_TE_interface_dispatch {

TEST_CASE("21_Tree any++ open method") {
  using namespace anyxx;

  auto expr = any_node<>{
      std::in_place_type<Times>, any_node<>{std::in_place_type<Integer>, 2},
      any_node<>{std::in_place_type<Plus>,
                 any_node<>{std::in_place_type<Integer>, 3},
                 any_node<>{std::in_place_type<Integer>, 4}}};

  // REQUIRE(&v_table_instance<node, Times>() == &get_v_table(expr));
  // REQUIRE(v_table_instance<node, Times>().size() >= 3u);
  // REQUIRE(v_table_instance<node, Times>()[0]);
  // REQUIRE(v_table_instance<node, Times>()[1]);
  // REQUIRE(v_table_instance<node, Times>()[2]);

  auto v = evaluate(expr);
  REQUIRE(v == 14);
  std::stringstream out;
  out << as_forth(expr) << " = " << as_lisp(expr) << " = " << evaluate(expr);
  std::cout << out.str() << "\n";
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");

#ifndef _DEBUG
  std::cout << "Ensure 'target_compile_options(examples_inline_mode PRIVATE "
               "/Ob2)' is used!\n";
  BENCHMARK("21_Tree any++ open method evaluate") { return evaluate(expr); };
  BENCHMARK("21_Tree any++ open method as_lisp") { return as_lisp(expr); };
#endif  // !_DEBUG
}

}  // namespace _21_Tree_TE_interface_dispatch