#include <cmath>
#include <iostream>
#include <map>
#include <string>

#include "../include/virtual_void/erased/data/has_no_meta/observer.h"
#include "../include/virtual_void/erased/interface/declare_macro.h"
#include "include/catch.hpp"

using namespace Catch::Matchers;

using namespace ::virtual_void::erased;

const double M_PI = 3.14;

struct position {
  float x, y;
};

struct X {
  std::string s_;
  std::string to_string() const { return s_; }
};

namespace {
//ERASED_INTERFACE(node_si_i, (INTERFACE_CONST_METHOD(const VALUE&, at, KEY),
//                             INTERFACE_CONST_METHOD(std::size_t, size)))
template <typename T, typename KEY, typename VALUE>
struct map_t_i_default_v_table_map {
  auto at(T const* x, KEY _sig) {
    return x->at(std::forward<decltype(_sig)>(_sig));
  };
  auto size(T const* x) { return x->size(); };
};
template <typename T, typename KEY, typename VALUE>
struct map_t_i_v_table_map
    : map_t_i_default_v_table_map<typename T, typename KEY, typename VALUE> {};
template <typename BASE_V_TABLE, typename KEY, typename VALUE>
struct map_t_iv_table : BASE_V_TABLE {
  using v_table_base_t = BASE_V_TABLE;
  using void_t = v_table_base_t::void_t;
  using v_table_t = map_t_iv_table;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : v_table_base_t::static_is_derived_from(from);
  }
  VALUE (*at)(void_t, KEY);
  std::size_t (*size)(void_t);
  ;
  template <typename UNERASE>
  map_t_iv_table(UNERASE unerase) : v_table_base_t(unerase) {
    using v_table_map = map_t_i_v_table_map<typename UNERASE::type,
                                            typename KEY, typename VALUE>;
    at = [](void_t _vp, KEY _sig) {
      return v_table_map{}.at((UNERASE{}(_vp)),
                              std::forward<decltype(_sig)>(_sig));
    };
    size = [](void_t _vp) { return v_table_map{}.size((UNERASE{}(_vp))); };
    ;
    ::virtual_void::erased::interface::set_is_derived_from<v_table_t>(this);
  };
};
template <typename VIRTUAL_VOID, typename KEY, typename VALUE>
struct map_t_i : ::virtual_void::erased::interface::base<VIRTUAL_VOID> {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using void_t = VIRTUAL_VOID::void_t;
  using base_t = ::virtual_void::erased::interface::base<VIRTUAL_VOID>;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t =
      map_t_iv_table<typename v_table_base_t, typename KEY, typename VALUE>;
  using query_v_table_unique_t = map_t_iv_table<
      typename ::virtual_void::erased::interface::base<virtual_void_t>,
      typename KEY, typename VALUE>;
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
  map_t_i(virtual_void_t virtual_void, v_table_t* v_table)
      : base_t(std::move(virtual_void), v_table) {}
  template <typename CONSTRUCTED_WITH>
  map_t_i(CONSTRUCTED_WITH&& v)
    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, base_t>)
      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
    static v_table_t imlpemented_v_table{
        ::virtual_void::erased::unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
    v_table_ = &imlpemented_v_table;
  }
  template <typename OTHER>
  map_t_i(const OTHER& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(other) {}
  VALUE at(KEY _sig) const
    requires(::virtual_void::erased::const_correct_for_virtual_void<
             void const, virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)->at(
        base_t::virtual_void_.data(), std::forward<decltype(_sig)>(_sig));
  }
  std::size_t size() const
    requires(::virtual_void::erased::const_correct_for_virtual_void<
             void const, virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)->size(
        base_t::virtual_void_.data());
  }
  map_t_i(const map_t_i&) = default;
  map_t_i(map_t_i&) = default;
  map_t_i(map_t_i&&) = default;

 protected:
  map_t_i() = default;
};


//WORKS!
//template <typename T, typename KEY, typename VALUE>
//struct map_t_i_default_v_table_map {
//  auto at(T const* x, KEY _sig) {
//    return x->at(std::forward<decltype(_sig)>(_sig));
//  };
//  auto size(T const* x) { return x->size(); };
//};
//template <typename T, typename KEY, typename VALUE>
//struct map_t_i_v_table_map
//    : map_t_i_default_v_table_map<typename T, typename KEY, typename VALUE> {};
//template <typename BASE_V_TABLE, typename KEY, typename VALUE>
//struct map_t_iv_table : BASE_V_TABLE {
//  using v_table_base_t = BASE_V_TABLE;
//  using void_t = v_table_base_t::void_t;
//  using v_table_t = map_t_iv_table;
//  static bool static_is_derived_from(const std::type_info& from) {
//    return typeid(v_table_t) == from
//               ? true
//               : v_table_base_t::static_is_derived_from(from);
//  }
//  VALUE (*at)(void_t, KEY);
//  std::size_t (*size)(void_t);
//  ;
//  template <typename UNERASE>
//  map_t_iv_table(UNERASE unerase) : v_table_base_t(unerase) {
//    using v_table_map = map_t_i_v_table_map<typename UNERASE::type,
//                                            KEY, VALUE>;
//    at = [](void_t _vp, KEY _sig) -> VALUE{
//      return v_table_map{}.at((UNERASE{}(_vp)),
//                              std::forward<decltype(_sig)>(_sig));
//    };
//    size = [](void_t _vp) -> std::size_t { return v_table_map{}.size((UNERASE{}(_vp))); };
//    ;
//    ::virtual_void::erased::interface::set_is_derived_from<v_table_t>(this);
//  };
//};
//template <typename VIRTUAL_VOID, typename KEY, typename VALUE>
//struct map_t_i : ::virtual_void::erased::interface::base<VIRTUAL_VOID> {
// public:
//  using virtual_void_t = VIRTUAL_VOID;
//  using void_t = VIRTUAL_VOID::void_t;
//  using base_t = ::virtual_void::erased::interface::base<VIRTUAL_VOID>;
//  using v_table_base_t = base_t::v_table_t;
//  using v_table_t =
//      map_t_iv_table<typename v_table_base_t, typename KEY, typename VALUE>;
//  using query_v_table_unique_t = map_t_iv_table<
//      typename ::virtual_void::erased::interface::base<virtual_void_t>,
//      typename KEY, typename VALUE>;
//  template <typename T>
//  using is_already_base =
//      std::conditional_t<std::is_same_v<T, query_v_table_unique_t>,
//                         std::true_type,
//                         typename base_t::template is_already_base<T>>;
//  static_assert(!base_t::is_already_base<query_v_table_unique_t>::value,
//                "A v_table may be instanciated only once per interface");
//
// protected:
//  using base_t::v_table_;
//  using base_t::virtual_void_;
//
// public:
//  map_t_i(virtual_void_t virtual_void, v_table_t* v_table)
//      : base_t(std::move(virtual_void), v_table) {}
//  template <typename CONSTRUCTED_WITH>
//  map_t_i(CONSTRUCTED_WITH&& v)
//    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, base_t>)
//      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
//    static v_table_t imlpemented_v_table{
//        ::virtual_void::erased::unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
//    v_table_ = &imlpemented_v_table;
//  }
//  template <typename OTHER>
//  map_t_i(const OTHER& other)
//    requires(std::derived_from<OTHER, base_t>)
//      : base_t(other) {}
//  const VALUE& at(KEY _sig) const
//    requires(::virtual_void::erased::const_correct_for_virtual_void<
//             void const, virtual_void_t>)
//  {
//    return static_cast<v_table_t*>(v_table_)->at(
//        base_t::virtual_void_.data(), std::forward<decltype(_sig)>(_sig));
//  }
//  std::size_t size() const
//    requires(::virtual_void::erased::const_correct_for_virtual_void<
//             void const, virtual_void_t>)
//  {
//    return static_cast<v_table_t*>(v_table_)->size(
//        base_t::virtual_void_.data());
//  }
//  map_t_i(const map_t_i&) = default;
//  map_t_i(map_t_i&) = default;
//  map_t_i(map_t_i&&) = default;
//
// protected:
//  map_t_i() = default;
//};
}  // namespace

TEST_CASE("erased template test") {
  std::map<std::string, int> map_string_to_int = {{"one", 1}, {"two", 2}};

  auto test_map_t_i =
      [](map_t_i<data::has_no_meta::const_observer, std::string, int> map_i) {
        REQUIRE(map_i.size() == 2);
        REQUIRE(map_i.at("one") == 1);
        REQUIRE(map_i.at("two") == 2);
      };
  test_map_t_i(map_string_to_int);
}