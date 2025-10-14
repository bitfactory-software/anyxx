// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <catch.hpp>
#include <iostream>
#include <string>
#include <anyxx/anyxx.hpp>

using std::cout;
using std::string;

using namespace anyxx;

namespace _21_Tree_any_borrow_as {

ANY(any_node,
             (ANY_CONST_METHOD(int, value), ANY_CONST_METHOD(string, as_forth)))
}  // namespace 21_Tree_any_borrow_as

namespace _21_Tree_any_borrow_as {
ANY(any_as_lisp, (ANY_CONST_METHOD(string, as_lisp)))
ANY(any_value2, (ANY_CONST_METHOD(int, value2)))
}  // namespace _21_Tree_any_borrow_as

namespace _21_Tree_any_borrow_as {

auto as_lisp_(auto const& v) {
  return borrow_as<any_as_lisp<const_observer>>(v)->as_lisp();
}
auto value2_(auto const& v) {
  return borrow_as<any_value2<const_observer>>(v)->value2();
}

struct Plus {
  Plus(any_node<shared_const> const& left, any_node<shared_const> const& right) : left(left), right(right) {}
  int value() const { return left.value() + right.value(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " +";
  }
  string as_lisp() const {
    return "(plus " + as_lisp_(left) + " " + as_lisp_(right) + ")";
  }
  int value2() const { return value2_(left) + value2_(right); }

  any_node<shared_const> left, right;
};

struct Times {
  Times(any_node<shared_const> const& left, any_node<shared_const> const& right) : left(left), right(right) {}
  int value() const { return left.value() * right.value(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " *";
  }
  string as_lisp() const {
    return "(times " + as_lisp_(left) + " " + as_lisp_(right) + ")";
  }
  int value2() const { return value2_(left) * value2_(right); }

  any_node<shared_const> left, right;
};

struct Integer {
  explicit Integer(int value) : int_(value) {}
  int value() const { return int_; }
  string as_forth() const { return std::to_string(int_); }
  string as_lisp() const { return std::to_string(int_); }
  int value2() const { return int_; }

  int int_;
};

template <typename NODE, typename... ARGS>
auto make_node(ARGS&&... args) {
  return std::make_shared<NODE>(std::forward<ARGS>(args)...);
}

}  // namespace _21_Tree_any_borrow_as

using namespace _21_Tree_any_borrow_as;

ANY_REGISTER_MODEL(Plus, any_as_lisp);
ANY_REGISTER_MODEL(Times, any_as_lisp);
ANY_REGISTER_MODEL(Integer, any_as_lisp);

ANY_REGISTER_MODEL(Plus, any_value2);
ANY_REGISTER_MODEL(Times, any_value2);
ANY_REGISTER_MODEL(Integer, any_value2);

TEST_CASE("_21_Tree_any_borrow_as") {
  using namespace anyxx;
  using namespace _21_Tree_any_borrow_as;

  auto& type_info = anyxx::get_meta_data<Times>();
  REQUIRE(type_info.get_i_table().size() == 2u);
  auto expr = any_node<shared_const>(make_node<Times>(
      make_node<Integer>(2),
      make_node<Plus>(make_node<Integer>(3), make_node<Integer>(4))));

  auto v = expr.value();
  REQUIRE(v == 14);
  std::stringstream out;
  out << expr.as_forth() << " = " << as_lisp_(expr) << " = " << value2_(expr);
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  BENCHMARK("_21_Tree_any_borrow_as value") { return expr.value(); };
  BENCHMARK("_21_Tree_any_borrow_as value2") { return value2_(expr); };
  BENCHMARK("_21_Tree_any_borrow_as as_lisp") { return as_lisp_(expr); };
#endif  // !_DEBUG
}
