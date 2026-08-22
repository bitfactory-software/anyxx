// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <bit_factory/anyxx.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <memory>
#include <string>

using namespace anyxx;

namespace {

ANY_(node, dynamic_copyable,
     (ANY_FN(int, value, (), const), ANY_FN(std::string, as_forth, (), const),
      ANY_FN(std::string, as_lisp, (), const)),
     cow)

struct Plus {
  Plus(any_node<> left, any_node<> right)
      : left_(std::move(left)), right_(std::move(right)) {}
  [[nodiscard]] int value() const { return left_.value() + right_.value(); }
  [[nodiscard]] std::string as_forth() const {
    return left_.as_forth() + " " + right_.as_forth() + " +";
  }
  [[nodiscard]] std::string as_lisp() const {
    return "(plus " + left_.as_lisp() + " " + right_.as_lisp() + ")";
  }

  any_node<> left_, right_;
};

struct Times {
  Times(any_node<> left, any_node<> right)
      : left_(std::move(left)), right_(std::move(right)) {}
  [[nodiscard]] int value() const { return left_.value() * right_.value(); }
  [[nodiscard]] std::string as_forth() const {
    return left_.as_forth() + " " + right_.as_forth() + " *";
  }
  [[nodiscard]] std::string as_lisp() const {
    return "(times " + left_.as_lisp() + " " + right_.as_lisp() + ")";
  }

  any_node<> left_, right_;
};

struct Integer {
  explicit Integer(int value) : int_(value) {}
  [[nodiscard]] int value() const { return int_; }
  [[nodiscard]] std::string as_forth() const { return std::to_string(int_); }
  [[nodiscard]] std::string as_lisp() const { return std::to_string(int_); }

  int int_;
};

}  // namespace

TEST_CASE("21_Tree_any") {
  using namespace anyxx;

  auto expr = any_node<>{Times{Integer(2), Plus{Integer(3), Integer(4)}}};

  auto v = expr.value();
  REQUIRE(v == 14);
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
