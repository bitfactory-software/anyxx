// virtual_void variant of this yomm2 example via c++RTTI
// https://github.com/jll63/yomm2/blob/master/examples/accept_no_visitors.cpp

#include <iostream>
#include <memory>
#include <string>

#include "../include/virtual_void/erased/data/shared_const_ptr.h"
#include "../include/virtual_void/erased/interface.h"
#include "../include/virtual_void/erased/lifetime/shared_const.h"
#include "include/catch.hpp"

using std::cout;
using std::string;

namespace {

template <typename T>
struct node_i_default_v_table_map {
  auto value_(T const* x) { return x->value_(); };
  auto as_forth(T const* x) { return x->as_forth(); };
  auto as_lisp(T const* x) { return x->as_lisp(); };
};
template <typename T>
struct node_i_v_table_map : node_i_default_v_table_map<T> {};
template <typename BASE_V_TABLE>
struct node_iv_table : BASE_V_TABLE {
  using v_table_base_t = BASE_V_TABLE;
  using void_t = v_table_base_t::void_t;
  using v_table_t = node_iv_table;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : v_table_base_t::static_is_derived_from(from);
  }
  int (*value_)(void_t);
  string (*as_forth)(void_t);
  string (*as_lisp)(void_t);
  template <typename UNERASE>
  node_iv_table(UNERASE unerase) : v_table_base_t(unerase) {
    using v_table_map = node_i_v_table_map<typename UNERASE::type>;
    value_ = [](void_t _vp) { return v_table_map{}.value_((UNERASE{}(_vp))); };
    as_forth = [](void_t _vp) {
      return v_table_map{}.as_forth((UNERASE{}(_vp)));
    };
    as_lisp = [](void_t _vp) {
      return v_table_map{}.as_lisp((UNERASE{}(_vp)));
    };
    ;
    virtual_void::erased::set_is_derived_from<v_table_t>(this);
  };
};
template <virtual_void::erased::is_virtual_void VIRTUAL_VOID>
struct node_i : virtual_void::erased::base<VIRTUAL_VOID> {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using void_t = VIRTUAL_VOID::void_t;
  using base_t = virtual_void::erased::base<VIRTUAL_VOID>;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t = node_iv_table<v_table_base_t>;
  using query_v_table_unique_t =
      node_iv_table<virtual_void::erased::base<virtual_void_t>>;
  template <typename T>
  using is_already_base =
      std::conditional_t<std::is_same_v<T, query_v_table_unique_t>,
                         std::true_type,
                         typename base_t::template is_already_base<T>>;
  static_assert(!base_t::is_already_base<query_v_table_unique_t>::value,
                "A v_table may be instanciated only once per interface");

 protected:
  using base_t::v_table_;
  using base_t::virtual_void_;

 public:
  node_i(virtual_void_t virtual_void, v_table_t* v_table)
      : base_t(std::move(virtual_void), v_table) {}
  template <typename CONSTRUCTED_WITH>
  node_i(CONSTRUCTED_WITH&& v)
    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, base_t>)
      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
    static v_table_t imlpemented_v_table{
        virtual_void::erased::unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
    v_table_ = &imlpemented_v_table;
  }
  template <typename OTHER>
  node_i(const OTHER& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(other) {}
  int value_() const
    requires(virtual_void::erased::const_correct_for_lifetime_holder<
             void const, virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)->value_(
        base_t::virtual_void_.data());
  }
  string as_forth() const
    requires(virtual_void::erased::const_correct_for_lifetime_holder<
             void const, virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)->as_forth(
        base_t::virtual_void_.data());
  }
  string as_lisp() const
    requires(virtual_void::erased::const_correct_for_lifetime_holder<
             void const, virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)->as_lisp(
        base_t::virtual_void_.data());
  }
  node_i(const node_i&) = default;
  node_i(node_i&) = default;
  node_i(node_i&&) = default;

 protected:
  node_i() = default;
};

using node = node_i<virtual_void::erased::shared_const>;

struct Plus {
  Plus(node left, node right) : left(left), right(right) {}
  int value_() const { return left.value_() + right.value_(); }
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
  int value_() const { return left.value_() * right.value_(); }
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
  int value_() const { return int_; }
  string as_forth() const { return std::to_string(int_); }
  string as_lisp() const { return std::to_string(int_); }

  int int_;
};

template <typename NODE, typename... ARGS>
auto make_node(ARGS&&... args) {
  return node{virtual_void::erased::make_shared_const_no_meta<NODE>(
      std::forward<ARGS>(args)...)};
}

}  // namespace

TEST_CASE("21_Tree_TE_dynamic_interface") {
  //    using virtual_void::m_table::make_shared_const;
  using namespace virtual_void;

  static_assert(erased::is_virtual_void<erased::typed_shared_const<Times>>);
  static_assert(erased::is_virtual_void<erased::typed_shared_const<Plus>>);
  static_assert(erased::is_virtual_void<erased::typed_shared_const<Integer>>);

  auto expr = node(make_node<Times>(
      make_node<Integer>(2),
      make_node<Plus>(make_node<Integer>(3), make_node<Integer>(4))));

  REQUIRE(expr.value_() == 14);
  std::stringstream out;
  out << expr.as_forth() << " = " << expr.as_lisp() << " = " << expr.value_();
  REQUIRE(out.str() == "2 3 4 + * = (times 2 (plus 3 4)) = 14");
  std::cout << out.str() << "\n";
#ifndef _DEBUG
  BENCHMARK("21_Tree_TE_dynamic_interface value") { return expr.value(); };
  BENCHMARK("21_Tree_TE_dynamic_interface as_lisp") { return expr.as_lisp(); };
#endif  // !_DEBUG
}
