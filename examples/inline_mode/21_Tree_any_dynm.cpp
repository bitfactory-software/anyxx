// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <bit_factory/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>

using namespace anyxx;

namespace {

ANY(node, (ANY_METHOD(int, value, (), const),
             ANY_METHOD(std::string, as_forth, (), const),
             ANY_METHOD(std::string, as_lisp, (), const)), shared_const, dynm)

struct Plus {
  Plus(node<> left, node<> right)
      : left_(std::move(left)), right_(std::move(right)) {}
  [[nodiscard]] int value() const { return left_.value() + right_.value(); }
  [[nodiscard]] std::string as_forth() const {
    return left_.as_forth() + " " + right_.as_forth() + " +";
  }
  [[nodiscard]] std::string as_lisp() const {
    return "(plus " + left_.as_lisp() + " " + right_.as_lisp() + ")";
  }

  node<> left_, right_;
};

struct Times {
  Times(node<> left, node<> right)
      : left_(std::move(left)), right_(std::move(right)) {}
  [[nodiscard]] int value() const { return left_.value() * right_.value(); }
  [[nodiscard]] std::string as_forth() const {
    return left_.as_forth() + " " + right_.as_forth() + " *";
  }
  [[nodiscard]] std::string as_lisp() const {
    return "(times " + left_.as_lisp() + " " + right_.as_lisp() + ")";
  }

  node<> left_, right_;
};

struct Integer {
  explicit Integer(int value) : int_(value) {}
  [[nodiscard]] int value() const { return int_; }
  [[nodiscard]] std::string as_forth() const { return std::to_string(int_); }
  [[nodiscard]] std::string as_lisp() const { return std::to_string(int_); }

  int int_;
};

template <typename NODE, typename... ARGS>
[[nodiscard]] auto make_node(ARGS&&... args) {
  return node<>{std::make_shared<NODE>(std::forward<ARGS>(args)...)};
}

}  // namespace

TEST_CASE("21_Tree_any_dynm") {
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
