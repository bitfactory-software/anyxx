// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <bit_factory/anyxx.hpp>
#include <bit_factory/anyxx/ext/dispatch.hpp>
#include <bit_factory/anyxx/ext/dispatch_table.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

using namespace anyxx;

namespace {

TRAIT_EX(node, anyxx::dynamic_copyable, , , , 
    (HAS_OPEN_DISPATCH), (using default_proxy_t = anyxx::cow))

struct Plus {
  any<node> left_, right_;
};
struct Times {
  any<node> left_, right_;
};
struct Integer {
  int i;
};

// =============================================================================
// add behavior to existing classes, without changing them

//-----------------------------------------------------------------------------
// open dispatch evaluate, returns int, dispatched via an any<node>;
dispatch<int(virtual_<any<node>>)> evaluate;
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
dispatch<std::string(virtual_<any<node>>)> as_forth;
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
dispatch<std::string(virtual_<any<node>>)> as_lisp;
auto __ = as_lisp.define<Plus>([](auto const& expr) {
  return "(plus " + as_lisp(expr.left_) + " " + as_lisp(expr.right_) + ")";
});
auto __ = as_lisp.define<Times>([](auto const& expr) {
  return "(times " + as_lisp(expr.left_) + " " + as_lisp(expr.right_) + ")";
});
auto __ = as_lisp.define<Integer>(
    [](auto const& expr) { return std::to_string(expr.i); });
//-----------------------------------------------------------------------------
}  // namespace

using namespace anyxx;

TEST_CASE("talk_23_9_no_visitors_open_dispatch") {
  using namespace anyxx;

  any<node> expr{Times{Integer(2), Plus{Integer{3}, Integer{4}}}};

  auto v = evaluate(expr);
  REQUIRE(v == 14);
  std::stringstream out;
  out << as_forth(expr) << " = " << as_lisp(expr) << " = " << evaluate(expr);
  std::cout << out.str() << "\n";
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");

#ifdef ANYXX_RUN_BENCHMARKS
  BENCHMARK("23_Tree any_++ open method evaluate") { return evaluate(expr); };
  BENCHMARK("23_Tree any_++ open method as_lisp") { return as_lisp(expr); };
#endif  // ANYXX_RUN_BENCHMARKS
}
