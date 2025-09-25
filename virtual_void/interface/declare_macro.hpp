#pragma once

#include <virtual_void/interface/base.hpp>
#include <virtual_void/meta/class.hpp>
#include <virtual_void/utillities/static_init.hpp>
#include <virtual_void/utillities/unnamed__.hpp>

#undef interface

//
// developed from:
// https://github.com/AlexCodesApps/virtual_void::blob/main/dynamic_v_table.hpp
//
// for Microsoft C++, you must enable the C-Preprocessor with this flag:
// /Zc:preprocessor (see CMakeLists.txt for example)
//

#define _detail_EXPAND(...) \
  _detail_EXPAND4(          \
      _detail_EXPAND4(_detail_EXPAND4(_detail_EXPAND4(__VA_ARGS__))))
#define _detail_EXPAND4(...) \
  _detail_EXPAND3(           \
      _detail_EXPAND3(_detail_EXPAND3(_detail_EXPAND3(__VA_ARGS__))))
#define _detail_EXPAND3(...) \
  _detail_EXPAND2(           \
      _detail_EXPAND2(_detail_EXPAND2(_detail_EXPAND2(__VA_ARGS__))))
#define _detail_EXPAND2(...) \
  _detail_EXPAND1(           \
      _detail_EXPAND1(_detail_EXPAND1(_detail_EXPAND1(__VA_ARGS__))))
#define _detail_EXPAND1(...) __VA_ARGS__

#define _detail_EXPAND_(...) \
  _detail_EXPAND_4(          \
      _detail_EXPAND_4(_detail_EXPAND_4(_detail_EXPAND_4(__VA_ARGS__))))
#define _detail_EXPAND_4(...) \
  _detail_EXPAND_3(           \
      _detail_EXPAND_3(_detail_EXPAND_3(_detail_EXPAND_3(__VA_ARGS__))))
#define _detail_EXPAND_3(...) \
  _detail_EXPAND_2(           \
      _detail_EXPAND_2(_detail_EXPAND_2(_detail_EXPAND_2(__VA_ARGS__))))
#define _detail_EXPAND_2(...) \
  _detail_EXPAND_1(           \
      _detail_EXPAND_1(_detail_EXPAND_1(_detail_EXPAND_1(__VA_ARGS__))))
#define _detail_EXPAND_1(...) __VA_ARGS__
#define _detail_RMCVREF(x)                                  \
  typename std::remove_const<typename std::remove_volatile< \
      typename std::remove_reference<x>::type>::type>::type
#define _detail_PARENS ()
#define _detail_foreach_macro_h(macro, a, ...) \
  macro(a)                                     \
      __VA_OPT__(_detail_foreach_macro_a _detail_PARENS(macro, __VA_ARGS__))
#define _detail_foreach_macro_a() _detail_foreach_macro_h
#define _detail_foreach_macro(macro, ...) \
  _detail_EXPAND(_detail_foreach_macro_h(macro, __VA_ARGS__))
#define _detail_map_macro_h(macro, a, ...) \
  macro(a) __VA_OPT__(, _detail_map_macro_a _detail_PARENS(macro, __VA_ARGS__))
#define _detail_map_macro(macro, ...) \
  _detail_EXPAND(_detail_map_macro_h(macro, __VA_ARGS__))
#define _detail_map_macro_a() _detail_map_macro_h
#define _detail_CONCAT_H(a, b) a##b
#define _detail_CONCAT(a, b) _detail_CONCAT_H(a, b)
#define _detail_PARAM_LIST_H(b, c, f, ...)              \
  std::forward<decltype(c)>(c)                          \
      __VA_OPT__(, _detail_PARAM_LIST_A _detail_PARENS( \
                       b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_PARAM_LIST_A() _detail_PARAM_LIST_H
#define _detail_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_PARAM_LIST_H(__VA_ARGS__))
#define _detail_PARAM_LIST_2H(b, c, f, ...)              \
  f c __VA_OPT__(, _detail_PARAM_LIST_2A _detail_PARENS( \
                       b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_PARAM_LIST_2A() _detail_PARAM_LIST_2H
#define _detail_PARAM_LIST2(...) \
  _detail_EXPAND_(_detail_PARAM_LIST_2H(__VA_ARGS__))
#define _detail_EXPAND_LIST(...) __VA_ARGS__

#define _detail_LEAD_COMMA_H(...) __VA_OPT__(, )
#define _detail_INTERFACE_FPD_H(l) _detail_INTERFACE_FUNCTION_PTR_DECL l
#define _detail_INTERFACE_MEMEBER_LIMP_H(l) \
  _detail_INTERFACE_LAMBDA_TO_MEMEBER_IMPL l
#define _detail_INTERFACE_FREE_LIMP_H(l) _detail_INTERFACE_LAMBDA_TO_FREE_IMPL l
#define _detail_INTERFACE_METHOD_H(l) _detail_INTERFACE_METHOD l

#define _detail_LEAD_COMMA_H_E(l) _detail_LEAD_COMMA_H l

#define _add_head_1(a, ...) a, __VA_ARGS__
#define _add_head(a, x) _add_head_1(a, _strip_braces x)
#define _strip_braces _strip_braces_1
#define _strip_braces_1(...) __VA_ARGS__

#define _typename _typename1
#define _typename1(t) t

#define _detail_INTERFACE_TEMPLATE_FORMAL_ARG_H(l) \
  _detail_INTERFACE_TEMPLATE_FORMAL_ARG l
#define _detail_INTERFACE_TEMPLATE_FORMAL_ARG(_typename) , typename _typename
#define _detail_INTERFACE_TEMPLATE_FORMAL_ARGS(...) \
  __VA_OPT__(_detail_INTERFACE_TEMPLATE_FORMAL_ARGS1(__VA_ARGS__))
#define _detail_INTERFACE_TEMPLATE_FORMAL_ARGS1(h, ...) \
  typename _typename h __VA_OPT__(                      \
      _detail_INTERFACE_TEMPLATE_FORMAL_ARGS2((__VA_ARGS__)))
#define _detail_INTERFACE_TEMPLATE_FORMAL_ARGS2(l)               \
  _detail_foreach_macro(_detail_INTERFACE_TEMPLATE_FORMAL_ARG_H, \
                        _detail_EXPAND_LIST l)

#define _detail_INTERFACE_TEMPLATE_ARG_H(l) _detail_INTERFACE_TEMPLATE_ARG l

#define _detail_INTERFACE_TEMPLATE_ARG(_typename) , _typename

#define _detail_INTERFACE_TEMPLATE_ARGS(...) \
  __VA_OPT__(_detail_INTERFACE_TEMPLATE_ARGS1(__VA_ARGS__))

#define _detail_INTERFACE_TEMPLATE_ARGS1(h, ...) \
  _typename h __VA_OPT__(_detail_INTERFACE_TEMPLATE_ARGS2((__VA_ARGS__)))

#define _detail_INTERFACE_TEMPLATE_ARGS2(l) \
  _detail_foreach_macro(_detail_INTERFACE_TEMPLATE_ARG_H, _detail_EXPAND_LIST l)

#define _detail_INTERFACE_V_TABLE_TEMPLATE_HEADER_H(...) \
  __VA_OPT__(template <_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(__VA_ARGS__)>)

#define _detail_INTERFACE_V_TABLE_TEMPLATE_HEADER(t) \
  _detail_INTERFACE_V_TABLE_TEMPLATE_HEADER_H t

#define _detail_INTERFACE_V_TABLE_TEMPLATE_FORMAL_ARGS_H(...) \
  __VA_OPT__(<_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(__VA_ARGS__)>)

#define _detail_INTERFACE_V_TABLE_TEMPLATE_FORMAL_ARGS(t) \
  _detail_INTERFACE_V_TABLE_TEMPLATE_FORMAL_ARGS_H t

#define _detail_TA_H(l) _detail_TA l

#define _detail_TA(type) , type

#define _detail_INTERFACE_MAP_LIMP_H(l) _detail_INTERFACE_MAP_IMPL l

#define _detail_INTERFACE_MAP_IMPL(type, name, const_, ...)               \
  auto name(T const_* x __VA_OPT__(                                       \
      , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {             \
    return x->name(__VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
  };

#define _detail_INTERFACE_FUNCTION_PTR_DECL(type, name, const_, ...) \
  type (*name)(void const_* __VA_OPT__(, __VA_ARGS__));

#define _detail_INTERFACE_LAMBDA_TO_MEMEBER_IMPL(type, name, const_, ...) \
  name = [](void const_* _vp __VA_OPT__(                                  \
             , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {      \
    return v_table_map{}.name(                                            \
        unchecked_unerase_cast<CONCRETE>(_vp) __VA_OPT__(, )              \
            __VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));        \
  };

#define _detail_INTERFACE_METHOD(type, name, const_, ...)                    \
  type name(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const     \
    requires(::virtual_void::const_correct_for_virtual_void<void const_*,    \
                                                            erased_data_t>) \
  {                                                                          \
    return static_cast<v_table_t*>(v_table_)->name(                          \
        virtual_void::get_data(base_t::erased_data_)                        \
            __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));         \
  }

#define VV_INTERFACE_TEMPLATE_(t, n, BASE, l)                                  \
  template <_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(_add_head((T), t))>         \
  struct n##_default_v_table_map {                                             \
    _detail_foreach_macro(_detail_INTERFACE_MAP_LIMP_H, _detail_EXPAND_LIST l) \
  };                                                                           \
  template <_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(_add_head((T), t))>         \
  struct n##_v_table_map                                                       \
      : n##_default_v_table_map<_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(        \
            _add_head((T), t))> {};                                            \
                                                                               \
  _detail_INTERFACE_V_TABLE_TEMPLATE_HEADER(t) struct n##_v_table              \
      : BASE##_v_table {                                                       \
    using v_table_base_t = BASE##_v_table;                                     \
    using v_table_t = n##_v_table;                                             \
    static bool static_is_derived_from(const std::type_info& from) {           \
      return typeid(v_table_t) == from                                         \
                 ? true                                                        \
                 : v_table_base_t::static_is_derived_from(from);               \
    }                                                                          \
    _detail_foreach_macro(_detail_INTERFACE_FPD_H, _detail_EXPAND_LIST l);     \
    template <typename CONCRETE>                                               \
    n##_v_table(std::in_place_type_t<CONCRETE> concrete)                       \
        : v_table_base_t(concrete) {                                           \
      using v_table_map = n##_v_table_map<_detail_INTERFACE_TEMPLATE_ARGS(     \
          _add_head((CONCRETE), t))>;                                          \
      _detail_foreach_macro(_detail_INTERFACE_MEMEBER_LIMP_H,                  \
                            _detail_EXPAND_LIST l);                            \
      ::virtual_void::interface::set_is_derived_from<v_table_t>(this);         \
      virtual_void::meta::runtime<virtual_void::meta::type_info, CONCRETE>()   \
          .register_v_table(this);                                             \
    };                                                                         \
                                                                               \
    template <typename CONCRETE>                                               \
    static auto imlpementation() {                                             \
      static n##_v_table v_table{std::in_place_type<CONCRETE>};                \
      return &v_table;                                                         \
    }                                                                          \
  };                                                                           \
                                                                               \
  template <_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(                            \
      _add_head((ERASED_DATA), t))>                                           \
  struct n : BASE<ERASED_DATA> {                                              \
    using erased_data_t = ERASED_DATA;                                       \
    using base_t = BASE<ERASED_DATA>;                                         \
    using v_table_base_t = base_t::v_table_t;                                  \
    using v_table_t =                                                          \
        n##_v_table _detail_INTERFACE_V_TABLE_TEMPLATE_FORMAL_ARGS(t);         \
                                                                               \
    template <typename CONCRETE>                                               \
    static auto v_table_imlpementation() {                                     \
      static_assert(!is_interface<CONCRETE>);                                  \
      return v_table_t::template imlpementation<CONCRETE>();                   \
    }                                                                          \
                                                                               \
    using base_t::erased_data_;                                               \
    using base_t::v_table_;                                                    \
                                                                               \
    n(erased_data_t virtual_void, v_table_t* v_table)                         \
        : base_t(std::move(virtual_void), v_table) {}                          \
    template <typename CONSTRUCTED_WITH>                                       \
    n(CONSTRUCTED_WITH&& v)                                                    \
      requires virtual_void::interface::constructibile_for<CONSTRUCTED_WITH,   \
                                                           ERASED_DATA>       \
        : base_t(std::forward<CONSTRUCTED_WITH>(v)) {                          \
      v_table_ = v_table_imlpementation<                                       \
          unerased_type<ERASED_DATA, CONSTRUCTED_WITH>>();                    \
    }                                                                          \
    template <typename CONSTRUCTED_WITH>                                       \
    n(const virtual_void::virtual_typed<CONSTRUCTED_WITH, erased_data_t>& vt) \
        : n(*vt) {}                                                            \
    template <typename OTHER>                                                  \
    n(const OTHER& other)                                                      \
      requires(std::derived_from<typename OTHER::v_table_t, v_table_t>)        \
        : base_t(other) {}                                                     \
    template <typename OTHER>                                                  \
    n(const OTHER&& other)                                                     \
      requires(std::derived_from<OTHER::v_table_t, v_table_t>)                 \
        : base_t(std::move(other)) {}                                          \
                                                                               \
    _detail_foreach_macro(_detail_INTERFACE_METHOD_H, _detail_EXPAND_LIST l)   \
                                                                               \
        auto const*                                                            \
        operator->() const {                                                   \
      return this;                                                             \
    }                                                                          \
    n() = default;                                                             \
    n(n const&) = default;                                                     \
    n(n&&) = default;                                                          \
    n& operator=(n const&) = default;                                          \
    n& operator=(n&&) = default;                                               \
    template <virtual_void::is_erased_data OTHER>                             \
    friend class virtual_void::interface::base;                                \
    template <typename TO, typename FROM>                                      \
    friend TO virtual_void::interface::unchecked_v_table_cast(FROM from)       \
      requires(std::derived_from<TO, FROM>);                                   \
  };

//    n(n&) = default;                                                           \

#define VV_INTERFACE_(n, BASE, l) VV_INTERFACE_TEMPLATE_((), n, BASE, l)

#define VV_INTERFACE(name, l) \
  VV_INTERFACE_(name, ::virtual_void::interface::base, l)

#define VV_INTERFACE_TEMPLATE(t, n, l) \
  VV_INTERFACE_TEMPLATE_(t, n, ::virtual_void::interface::base, l)

#define VV_METHOD_(...) (__VA_ARGS__)

#define VV_METHOD(ret, name, ...) VV_METHOD_(ret, name, , __VA_ARGS__)

#define VV_CONST_METHOD(ret, name, ...) \
  VV_METHOD_(ret, name, const, __VA_ARGS__)
