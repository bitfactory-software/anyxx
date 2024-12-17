// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <catch.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <virtual_void/data/has_i_table/observer.hpp>
#include <virtual_void/data/has_i_table/shared_const.hpp>
#include <virtual_void/data/has_no_meta/shared_const.hpp>
#include <virtual_void/interface/base.hpp>
#include <virtual_void/interface/conversion.hpp>
#include <virtual_void/interface/declare_macro.hpp>
#include <virtual_void/interface/i_table.hpp>

using std::cout;
using std::string;

using namespace virtual_void;
using namespace virtual_void::data::has_i_table;

namespace _21_Tree_TE_erased_interface_w_cast {

ERASED_INTERFACE(node_i, (INTERFACE_CONST_METHOD(int, value),
                          INTERFACE_CONST_METHOD(string, as_forth)))
using node = node_i<shared_const>;
}  // namespace _21_Tree_TE_erased_interface_w_cast

namespace _21_Tree_TE_erased_interface_w_cast {
ERASED_INTERFACE(lisp_i, (INTERFACE_CONST_METHOD(string, as_lisp)))
}

namespace _21_Tree_TE_erased_interface_w_cast {

auto as_lisp_(auto const& v) {
  return attach_interface<lisp_i<const_observer>>(v).as_lisp();
}

struct Plus {
  Plus(node left, node right) : left(left), right(right) {}
  int value() const { return left.value() + right.value(); }
  string as_forth() const {
    return left.as_forth() + " " + right.as_forth() + " +";
  }
  string as_lisp() const {
    return "(plus " + as_lisp_(left) + " " + as_lisp_(right) + ")";
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
    return "(times " + as_lisp_(left) + " " + as_lisp_(right) + ")";
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
  return NODE{std::forward<ARGS>(args)...};
}

}  // namespace _21_Tree_TE_erased_interface_w_cast

using namespace _21_Tree_TE_erased_interface_w_cast;

VV_DECLARE_V_TABLE_INDEX(, _21_Tree_TE_erased_interface_w_cast::node_i)
VV_DECLARE_V_TABLE_INDEX(, _21_Tree_TE_erased_interface_w_cast::lisp_i)

VV_I_TABLE_OF(Plus)
VV_I_TABLE_OF(Times)
VV_I_TABLE_OF(Integer)

virtual_void::interface::is_a<Plus, lisp_i_v_table> __;
virtual_void::interface::is_a<Times, lisp_i_v_table> __;
virtual_void::interface::is_a<Integer, lisp_i_v_table> __;

TEST_CASE("21_Tree_TE_erased_interface_w_cast") {
  using namespace virtual_void;
  using namespace _21_Tree_TE_erased_interface_w_cast;

  auto expr = node(make_node<Times>(
      make_node<Integer>(2),
      make_node<Plus>(make_node<Integer>(3), make_node<Integer>(4))));

  auto v = expr.value();
  REQUIRE(v == 14);
  std::stringstream out;
  out << expr.as_forth() << " = " << as_lisp_(expr) << " = " << expr.value();
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_dynamic_interface value") { return expr.value(); };
  BENCHMARK("21_Tree_TE_dynamic_interface as_lisp") { return as_lisp_(expr); };
#endif  // !_DEBUG
}

VV_DEFINE_V_TABLE_INDEX(_21_Tree_TE_erased_interface_w_cast ::node_i)
VV_DEFINE_V_TABLE_INDEX(_21_Tree_TE_erased_interface_w_cast ::lisp_i)
