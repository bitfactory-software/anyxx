// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using std::string;

namespace {

struct Node {
  virtual ~Node() = default;  // generates c++ vtable + type_info
  Node() = default;
  Node(Node const&) = default;
  Node& operator=(Node const&) = default;
  Node(Node&&) = default;
  Node& operator=(Node&&) = default;
  [[nodiscard]] virtual int value() const = 0;
  [[nodiscard]] virtual std::string as_forth() const = 0;
  [[nodiscard]] virtual std::string as_lisp() const = 0;
};

using shared_const_node = std::shared_ptr<const Node>;

struct Plus : Node {
  // NOLINTNEXTLINE
  Plus(shared_const_node const& left, shared_const_node const& right)
      : left(left), right(right) {}  // NOLINT
  [[nodiscard]] int value() const override {
    return left->value() + right->value();
  }
  [[nodiscard]] std::string as_forth() const override {
    return left->as_forth() + " " + right->as_forth() + " +";
  }
  [[nodiscard]] std::string as_lisp() const override {
    return "(plus " + left->as_lisp() + " " + right->as_lisp() + ")";
  }

  shared_const_node left, right;
};

struct Times : Node {
  // NOLINTNEXTLINE
  Times(shared_const_node const& left, shared_const_node const& right)
      : left(left), right(right) {}  // NOLINT
  [[nodiscard]] int value() const override {
    return left->value() * right->value();
  }
  [[nodiscard]] std::string as_forth() const override {
    return left->as_forth() + " " + right->as_forth() + " *";
  }
  [[nodiscard]] std::string as_lisp() const override {
    return "(times " + left->as_lisp() + " " + right->as_lisp() + ")";
  }

  shared_const_node left, right;
};

struct Integer : Node {
  explicit Integer(int value) : int_(value) {}
  [[nodiscard]] int value() const override { return int_; }
  [[nodiscard]] std::string as_forth() const override {
    return std::to_string(int_);
  }
  [[nodiscard]] std::string as_lisp() const override {
    return std::to_string(int_);
  }

  int int_;
};

}  // namespace

TEST_CASE("20_Tree_OO") {
  using std::make_shared;

  auto expr = make_shared<Times>(
      make_shared<Integer>(2),
      make_shared<Plus>(make_shared<Integer>(3), make_shared<Integer>(4)));

  REQUIRE(expr->value() == 14);
  std::stringstream out;
  out << expr->as_forth() << " = " << expr->as_lisp() << " = " << expr->value();
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  BENCHMARK("20_Tree_OO value") { return expr->value(); };
  BENCHMARK("20_Tree_OO as_lisp") { return expr->as_lisp(); };
#endif  // !_DEBUG
}
