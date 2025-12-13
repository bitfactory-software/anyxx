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
      : left_(left), right_(right) {}  // NOLINT
  [[nodiscard]] int value() const override {
    return left_->value() + right_->value();
  }
  [[nodiscard]] std::string as_forth() const override {
    return left_->as_forth() + " " + right_->as_forth() + " +";
  }
  [[nodiscard]] std::string as_lisp() const override {
    return "(plus " + left_->as_lisp() + " " + right_->as_lisp() + ")";
  }

  shared_const_node left_, right_;
};

struct Times : Node {
  // NOLINTNEXTLINE
  Times(shared_const_node const& left, shared_const_node const& right)
      : left_(left), right_(right) {}  // NOLINT
  [[nodiscard]] int value() const override {
    return left_->value() * right_->value();
  }
  [[nodiscard]] std::string as_forth() const override {
    return left_->as_forth() + " " + right_->as_forth() + " *";
  }
  [[nodiscard]] std::string as_lisp() const override {
    return "(times " + left_->as_lisp() + " " + right_->as_lisp() + ")";
  }

  shared_const_node left_, right_;
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

TEST_CASE("20_Tree virtual function") {
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
  BENCHMARK("20_Tree virtual function value") { return expr->value(); };
  BENCHMARK("20_Tree virtual function as_lisp") { return expr->as_lisp(); };
#endif  // !_DEBUG
}
