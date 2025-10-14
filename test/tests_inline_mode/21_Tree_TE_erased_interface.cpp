// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <catch.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <anypp/any++.hpp>

using std::cout;
using std::string;

using namespace anypp;
using namespace anypp;

namespace {

ANY(node_i,
             (ANY_CONST_METHOD(int, value),
              ANY_CONST_METHOD(string, as_forth),
              ANY_CONST_METHOD(string, as_lisp)))

using node = node_i<shared_const>;

struct Plus {
  Plus(node left, node right) : left(left), right(right) {}
  int value() const { return left.value() + right.value(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " +";
  }
  string as_lisp() const {
    return "(plus " + left.as_lisp() + " " + right.as_lisp() + ")";
  }

  node left, right;
};

struct Times {
  Times(node left, node right) : left(left), right(right) {}
  int value() const { return left.value() * right.value(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " *";
  }
  string as_lisp() const {
    return "(times " + left.as_lisp() + " " + right.as_lisp() + ")";
  }

  node left, right;
};

struct Integer {
  explicit Integer(int value) : int_(value) {}
  int value() const { return int_; }
  string as_forth() const { return std::to_string(int_); }
  string as_lisp() const { return std::to_string(int_); }

  int int_;
};

template <typename NODE, typename... ARGS>
auto make_node(ARGS&&... args) {
  return node{std::make_shared<NODE>(std::forward<ARGS>(args)...)};
}

}  // namespace


TEST_CASE("21_Tree_TE_dynamic_interface") {
  using namespace anypp;

  auto expr = node(make_node<Times>(
      make_node<Integer>(2),
      make_node<Plus>(make_node<Integer>(3), make_node<Integer>(4))));

  REQUIRE(expr.value() == 14);
  std::stringstream out;
  out << expr.as_forth() << " = " << expr.as_lisp() << " = " << expr.value();
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_dynamic_interface value") { return expr.value(); };
  BENCHMARK("21_Tree_TE_dynamic_interface as_lisp") { return expr.as_lisp(); };
#endif  // !_DEBUG
}
