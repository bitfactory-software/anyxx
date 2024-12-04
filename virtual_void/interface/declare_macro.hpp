#pragma once

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

#define _detail_TA_H(l) _detail_TA l

#define _detail_TA(type) , type

#define _detail_INTERFACE_MAP_LIMP_H(l) _detail_INTERFACE_MAP_IMPL l

#define _detail_INTERFACE_MAP_IMPL(type, name, const_, ...)               \
  auto name(T const_* x __VA_OPT__(                                       \
      , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {             \
    return x->name(__VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
  };

#define _detail_INTERFACE_FUNCTION_PTR_DECL(type, name, const_, ...) \
  type (*name)(void_t __VA_OPT__(, __VA_ARGS__));

#define _detail_INTERFACE_LAMBDA_TO_MEMEBER_IMPL(type, name, const_, ...) \
  name = [](void_t _vp __VA_OPT__(                                        \
             , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {      \
    return v_table_map{}.name((UNERASE{}(_vp))__VA_OPT__(, ) __VA_OPT__(  \
        _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));                       \
  };

#define _detail_INTERFACE_METHOD(type, name, const_, ...)                    \
  type name(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const     \
    requires(::virtual_void::const_correct_for_virtual_void<void const_*,    \
                                                            virtual_void_t>) \
  {                                                                          \
    return static_cast<v_table_t*>(v_table_)->name(                          \
        virtual_void::get_data(base_t::virtual_void_)                        \
            __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));         \
  }

#define ERASED_INTERFACE_TEMPLATE_(t, n, BASE, l)                              \
  template <_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(_add_head((T), t))>         \
  struct n##_default_v_table_map {                                             \
    _detail_foreach_macro(_detail_INTERFACE_MAP_LIMP_H, _detail_EXPAND_LIST l) \
  };                                                                           \
  template <_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(_add_head((T), t))>         \
  struct n##_v_table_map                                                       \
      : n##_default_v_table_map<_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(        \
            _add_head((T), t))> {};                                            \
                                                                               \
  template <_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(                            \
      _add_head((BASE_V_TABLE), t))>                                           \
  struct n##_v_table : BASE_V_TABLE {                                          \
    using v_table_base_t = BASE_V_TABLE;                                       \
    using void_t = v_table_base_t::void_t;                                     \
    using v_table_t = n##_v_table;                                             \
    static bool static_is_derived_from(const std::type_info& from) {           \
      return typeid(v_table_t) == from                                         \
                 ? true                                                        \
                 : v_table_base_t::static_is_derived_from(from);               \
    }                                                                          \
    _detail_foreach_macro(_detail_INTERFACE_FPD_H, _detail_EXPAND_LIST l);     \
    template <typename UNERASE>                                                \
    n##_v_table(UNERASE unerase) : v_table_base_t(unerase) {                   \
      using v_table_map = n##_v_table_map<_detail_INTERFACE_TEMPLATE_ARGS(     \
          _add_head((typename UNERASE::type), t))>;                            \
      _detail_foreach_macro(_detail_INTERFACE_MEMEBER_LIMP_H,                  \
                            _detail_EXPAND_LIST l);                            \
      ::virtual_void::interface::set_is_derived_from<v_table_t>(this);         \
    };                                                                         \
  };                                                                           \
                                                                               \
  template <_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(                            \
      _add_head((VIRTUAL_VOID), t))>                                           \
  struct n : BASE<VIRTUAL_VOID> {                                              \
    using virtual_void_t = VIRTUAL_VOID;                                       \
    using base_t = BASE<VIRTUAL_VOID>;                                         \
    using void_t = typename base_t::void_t;                                    \
    using v_table_base_t = base_t::v_table_t;                                  \
    using v_table_t = n##_v_table<_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(      \
        _add_head((v_table_base_t), t))>;                                      \
    using query_v_table_unique_t =                                             \
        n##_v_table<_detail_INTERFACE_TEMPLATE_FORMAL_ARGS(                    \
            _add_head((::virtual_void::interface::base<virtual_void_t>), t))>; \
    template <typename T>                                                      \
    using is_already_base =                                                    \
        std::conditional_t<std::is_same_v<T, query_v_table_unique_t>,          \
                           std::true_type,                                     \
                           typename base_t::template is_already_base<T>>;      \
    static_assert(                                                             \
        !base_t::template is_already_base<query_v_table_unique_t>::value,      \
        "A v_table may be instanciated only once per interface");              \
                                                                               \
    template <typename CONSTRUCTED_WITH>                                       \
    static inline v_table_t imlpemented_v_table{                               \
        ::virtual_void::unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};            \
                                                                               \
    using base_t::virtual_void_;                                               \
    using base_t::v_table_;                                                    \
                                                                               \
    n(virtual_void_t virtual_void, v_table_t* v_table)                         \
        : base_t(std::move(virtual_void), v_table) {}                          \
    template <typename CONSTRUCTED_WITH>                                       \
    n(CONSTRUCTED_WITH&& v)                                                    \
      requires virtual_void::interface::constructibile_for<CONSTRUCTED_WITH,   \
                                                           VIRTUAL_VOID>       \
        : base_t(std::forward<CONSTRUCTED_WITH>(v)) {                          \
      v_table_ = &imlpemented_v_table<CONSTRUCTED_WITH>;                       \
    }                                                                          \
    template <typename CONSTRUCTED_WITH>                                       \
    n(const virtual_void::virtual_typed<CONSTRUCTED_WITH, virtual_void_t>& vt) \
        : n(*vt) {}                                                            \
    template <typename OTHER>                                                  \
    n(const OTHER& other)                                                      \
      requires(std::derived_from<OTHER, base_t>)                               \
        : base_t(other) {}                                                     \
    template <typename OTHER>                                                  \
    n(const OTHER&& other)                                                     \
      requires(std::derived_from<OTHER, base_t>)                               \
        : base_t(std::move(other)) {}                                          \
    _detail_foreach_macro(_detail_INTERFACE_METHOD_H, _detail_EXPAND_LIST l)   \
                                                                               \
        n() = default;                                                         \
    n(n const&) = default;                                                     \
    n(n&&) = default;                                                          \
    n& operator=(n const&) = default;                                          \
    n& operator=(n&&) = default;                                               \
    template <virtual_void::is_virtual_void OTHER>                             \
    friend class virtual_void::interface::base;                                \
    template <typename TO, typename FROM>                                      \
    friend TO virtual_void::interface::unchecked_v_table_cast(FROM from)       \
      requires(std::derived_from<TO, FROM>);                                   \
  };

//    n(n&) = default;                                                           \

#define ERASED_INTERFACE_(n, BASE, l) ERASED_INTERFACE_TEMPLATE_((), n, BASE, l)

#define ERASED_INTERFACE(name, l) \
  ERASED_INTERFACE_(name, ::virtual_void::interface::base, l)

#define ERASED_INTERFACE_TEMPLATE(t, n, l) \
  ERASED_INTERFACE_TEMPLATE_(t, n, ::virtual_void::interface::base, l)

#define INTERFACE_METHOD_(...) (__VA_ARGS__)

#define INTERFACE_METHOD(ret, name, ...) \
  INTERFACE_METHOD_(ret, name, , __VA_ARGS__)

#define INTERFACE_CONST_METHOD(ret, name, ...) \
  INTERFACE_METHOD_(ret, name, const, __VA_ARGS__)
