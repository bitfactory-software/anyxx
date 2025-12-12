// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <bit_factory/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

using std::string;

using namespace anyxx;

namespace {

ANY(node_i,
    (ANY_METHOD(int, value, (), const), ANY_METHOD(string, as_forth, (), const),
     ANY_METHOD(string, as_lisp, (), const)))
using node = node_i<shared_const, anyxx::dyn>;

struct Plus {
  Plus(node left, node right)
      : left(std::move(left)), right(std::move(right)) {}
  [[nodiscard]] int value() const { return left.value() + right.value(); }
  [[nodiscard]] string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " +";
  }
  [[nodiscard]] string as_lisp() const {
    return "(plus " + left.as_lisp() + " " + right.as_lisp() + ")";
  }

  node left, right;
};

struct Times {
  Times(node left, node right)
      : left(std::move(left)), right(std::move(right)) {}
  [[nodiscard]] int value() const { return left.value() * right.value(); }
  [[nodiscard]] string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " *";
  }
  [[nodiscard]] string as_lisp() const {
    return "(times " + left.as_lisp() + " " + right.as_lisp() + ")";
  }

  node left, right;
};

struct Integer {
  explicit Integer(int value) : int_(value) {}
  [[nodiscard]] int value() const { return int_; }
  [[nodiscard]] string as_forth() const { return std::to_string(int_); }
  [[nodiscard]] string as_lisp() const { return std::to_string(int_); }

  int int_;
};

template <typename NODE, typename... ARGS>
[[nodiscard]] auto make_node(ARGS&&... args) {
  return node{std::make_shared<NODE>(std::forward<ARGS>(args)...)};
}

}  // namespace

TEST_CASE("21_Tree_any_inline") {
  using namespace anyxx;
  auto expr = make_node<Times>(
      make_node<Integer>(2),
      make_node<Plus>(make_node<Integer>(3), make_node<Integer>(4)));
  REQUIRE(expr.value() == 14);
  std::stringstream out;
  out << expr.as_forth() << " = " << expr.as_lisp() << " = " << expr.value();
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  std::cout << "Ensure 'target_compile_options(examples_inline_mode PRIVATE "
               "/Ob2)' is used!\n";
  BENCHMARK("21_Tree any++ value") { return expr.value(); };
  BENCHMARK("21_Tree any++ as_lisp") { return expr.as_lisp(); };
#endif  // !_DEBUG
}
