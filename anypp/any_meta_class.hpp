#pragma once

#include <anypp/any_base.hpp>
#include <anypp/meta_data.hpp>
#include <anypp/utillities/unnamed__.hpp>

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
#define _detail_ANYPP_FPD_H(l) _detail_ANYPP_FUNCTION_PTR_DECL l
#define _detail_ANYPP_MEMEBER_LIMP_H(l) \
  _detail_ANYPP_LAMBDA_TO_MEMEBER_IMPL l
#define _detail_ANYPP_FREE_LIMP_H(l) _detail_ANYPP_LAMBDA_TO_FREE_IMPL l
#define _detail_ANYPP_METHOD_H(l) _detail_ANYPP_METHOD l

#define _detail_LEAD_COMMA_H_E(l) _detail_LEAD_COMMA_H l

#define _add_head_1(a, ...) a, __VA_ARGS__
#define _add_head(a, x) _add_head_1(a, _strip_braces x)
#define _strip_braces _strip_braces_1
#define _strip_braces_1(...) __VA_ARGS__

#define _typename _typename1
#define _typename1(t) t

#define _detail_ANYPP_TEMPLATE_FORMAL_ARG_H(l) \
  _detail_ANYPP_TEMPLATE_FORMAL_ARG l
#define _detail_ANYPP_TEMPLATE_FORMAL_ARG(_typename) , typename _typename
#define _detail_ANYPP_TEMPLATE_FORMAL_ARGS(...) \
  __VA_OPT__(_detail_ANYPP_TEMPLATE_FORMAL_ARGS1(__VA_ARGS__))
#define _detail_ANYPP_TEMPLATE_FORMAL_ARGS1(h, ...) \
  typename _typename h __VA_OPT__(                      \
      _detail_ANYPP_TEMPLATE_FORMAL_ARGS2((__VA_ARGS__)))
#define _detail_ANYPP_TEMPLATE_FORMAL_ARGS2(l)               \
  _detail_foreach_macro(_detail_ANYPP_TEMPLATE_FORMAL_ARG_H, \
                        _detail_EXPAND_LIST l)

#define _detail_ANYPP_TEMPLATE_ARG_H(l) _detail_ANYPP_TEMPLATE_ARG l

#define _detail_ANYPP_TEMPLATE_ARG(_typename) , _typename

#define _detail_ANYPP_TEMPLATE_ARGS(...) \
  __VA_OPT__(_detail_ANYPP_TEMPLATE_ARGS1(__VA_ARGS__))

#define _detail_ANYPP_TEMPLATE_ARGS1(h, ...) \
  _typename h __VA_OPT__(_detail_ANYPP_TEMPLATE_ARGS2((__VA_ARGS__)))

#define _detail_ANYPP_TEMPLATE_ARGS2(l) \
  _detail_foreach_macro(_detail_ANYPP_TEMPLATE_ARG_H, _detail_EXPAND_LIST l)

#define _detail_ANYPP_V_TABLE_TEMPLATE_HEADER_H(...) \
  __VA_OPT__(template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(__VA_ARGS__)>)

#define _detail_ANYPP_V_TABLE_TEMPLATE_HEADER(t) \
  _detail_ANYPP_V_TABLE_TEMPLATE_HEADER_H t

#define _detail_ANYPP_V_TABLE_TEMPLATE_INSTANCE_H(...) \
  __VA_OPT__(<_detail_ANYPP_TEMPLATE_ARGS(__VA_ARGS__)>)

#define _detail_ANYPP_V_TABLE_TEMPLATE_INSTANCE(t) \
  _detail_ANYPP_V_TABLE_TEMPLATE_INSTANCE_H t

#define _detail_ANYPP_V_TABLE_TEMPLATE_FORMAL_ARGS_H(...) \
  __VA_OPT__(<_detail_ANYPP_TEMPLATE_FORMAL_ARGS(__VA_ARGS__)>)

#define _detail_ANYPP_V_TABLE_TEMPLATE_FORMAL_ARGS(t) \
  _detail_ANYPP_V_TABLE_TEMPLATE_FORMAL_ARGS_H t

#define _detail_TA_H(l) _detail_TA l

#define _detail_TA(type) , type

#define _detail_ANYPP_MAP_LIMP_H(l) _detail_ANYPP_MAP_IMPL l

#define _detail_ANYPP_MAP_IMPL(type, name, name_ext, exact_const, const_, \
                                   ...)                                       \
  auto name(T const_* x __VA_OPT__(                                           \
      , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {                 \
    return (*x).name_ext(                                                     \
        __VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));                \
  };

#define _detail_ANYPP_FUNCTION_PTR_DECL(type, name, name_ext, exact_const, \
                                            const_, ...)                       \
  type (*name)(void const_* __VA_OPT__(, __VA_ARGS__));

#define _detail_ANYPP_LAMBDA_TO_MEMEBER_IMPL(type, name, name_ext,     \
                                                 exact_const, const_, ...) \
  name = [](void const_* _vp __VA_OPT__(                                   \
             , _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) -> type {       \
    return v_table_map{}.name(                                             \
        anypp::unchecked_unerase_cast<CONCRETE>(_vp) __VA_OPT__(, ) \
            __VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));         \
  };

#define _detail_ANYPP_METHOD(type, name, name_ext, exact_const, const_,  \
                                 ...)                                        \
  type name_ext(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const \
    requires(::anypp::const_correct_call_for_erased_data<             \
             void const_*, erased_data_t, exact_const>)                      \
  {                                                                          \
    return static_cast<v_table_t*>(v_table_)->name(                          \
        anypp::get_void_data_ptr(base_t::erased_data_)                \
            __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));         \
  }

#define _detail_ANYPP_MAP_FUNCTIONS(...)                     \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYPP_MAP_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYPP_V_TABLE_FUNCTION_PTRS(...)        \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYPP_FPD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYPP_V_TABLE_LAMBDAS(...)                       \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYPP_MEMEBER_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYPP_METHODS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYPP_METHOD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define ANY_TEMPLATE_(t, n, BASE, l)                                        \
                                                                               \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(                            \
      _add_head((ERASED_DATA), t))>                                            \
  struct n;                                                                    \
                                                                               \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(_add_head((T), t))>         \
  struct n##_default_v_table_map {                                             \
    _detail_ANYPP_MAP_FUNCTIONS(l)                                         \
  };                                                                           \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(_add_head((T), t))>         \
  struct n##_v_table_map                                                       \
      : n##_default_v_table_map<_detail_ANYPP_TEMPLATE_FORMAL_ARGS(        \
            _add_head((T), t))> {};                                            \
                                                                               \
  _detail_ANYPP_V_TABLE_TEMPLATE_HEADER(t) struct n##_v_table;             \
                                                                               \
  _detail_ANYPP_V_TABLE_TEMPLATE_HEADER(t) struct n##_v_table              \
      : BASE##_v_table,                                                        \
        anypp::extension_method_holder<                                 \
            anypp::has_extension_methods<n>, n> {                       \
    using v_table_base_t = BASE##_v_table;                                     \
    using v_table_t = n##_v_table;                                             \
    using own_extension_method_holder_t =                                      \
        anypp::extension_method_holder<                                 \
            anypp::has_extension_methods<n>, n>;                        \
                                                                               \
    static bool static_is_derived_from(const std::type_info& from) {           \
      return typeid(v_table_t) == from                                         \
                 ? true                                                        \
                 : v_table_base_t::static_is_derived_from(from);               \
    }                                                                          \
                                                                               \
    _detail_ANYPP_V_TABLE_FUNCTION_PTRS(l);                                \
                                                                               \
    static constexpr bool extension_methods_enabled =                          \
        anypp::has_extension_methods<n>;                                \
                                                                               \
    template <typename CONCRETE>                                               \
    n##_v_table(std::in_place_type_t<CONCRETE> concrete)                       \
        : v_table_base_t(concrete) {                                           \
      using v_table_map = n##_v_table_map<_detail_ANYPP_TEMPLATE_ARGS(     \
          _add_head((CONCRETE), t))>;                                          \
                                                                               \
      _detail_ANYPP_V_TABLE_LAMBDAS(l);                                    \
                                                                               \
      if constexpr (extension_methods_enabled) {                               \
        own_extension_method_holder_t::extension_method_table =                \
            ::anypp::extension_method_table_instance<n##_v_table,       \
                                                            CONCRETE>();       \
      }                                                                        \
                                                                               \
      ::anypp::set_is_derived_from<v_table_t>(this);                    \
    };                                                                         \
                                                                               \
    template <typename CONCRETE>                                               \
    static auto imlpementation() {                                             \
      return anypp::v_table_instance_implementaion<v_table_t,           \
                                                          CONCRETE>();         \
    }                                                                          \
  };                                                                           \
                                                                               \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(                            \
      _add_head((ERASED_DATA), t))>                                            \
  struct n : BASE<ERASED_DATA> {                                               \
    using erased_data_t = ERASED_DATA;                                         \
    using base_t = BASE<ERASED_DATA>;                                          \
    using v_table_base_t = base_t::v_table_t;                                  \
    using v_table_t =                                                          \
        n##_v_table _detail_ANYPP_V_TABLE_TEMPLATE_FORMAL_ARGS(t);         \
                                                                               \
    template <typename CONCRETE>                                               \
    static auto v_table_imlpementation() {                                     \
      static_assert(!anypp::is_any<CONCRETE>);                          \
      return v_table_t::template imlpementation<CONCRETE>();                   \
    }                                                                          \
                                                                               \
    using base_t::erased_data_;                                                \
    using base_t::v_table_;                                                    \
                                                                               \
    n(erased_data_t virtual_void, v_table_t* v_table)                          \
        : base_t(std::move(virtual_void), v_table) {}                          \
    template <typename CONSTRUCTED_WITH>                                       \
    n(CONSTRUCTED_WITH&& v)                                                    \
      requires anypp::constructibile_for<CONSTRUCTED_WITH, ERASED_DATA> \
        : base_t(std::forward<CONSTRUCTED_WITH>(v)) {                          \
      v_table_ = v_table_t::template imlpementation<                           \
          anypp::unerased<ERASED_DATA, CONSTRUCTED_WITH>>();            \
    }                                                                          \
    template <typename OTHER>                                                  \
    n(const OTHER& other)                                                      \
      requires(std::derived_from<typename OTHER::v_table_t, v_table_t> &&      \
               anypp::borrowable_from<erased_data_t,                    \
                                             typename OTHER::erased_data_t>)   \
        : base_t(other) {}                                                     \
    template <anypp::is_any OTHER>                                      \
    n(OTHER&& other) noexcept                                                  \
      requires(std::derived_from<OTHER::v_table_t, v_table_t> &&               \
               anypp::moveable_from<erased_data_t,                      \
                                           typename OTHER::erased_data_t>)     \
        : base_t(std::move(other)) {}                                          \
                                                                               \
    _detail_ANYPP_METHODS(l)                                               \
                                                                               \
        auto const*                                                            \
        operator->() const {                                                   \
      return this;                                                             \
    }                                                                          \
                                                                               \
    using base_t::operator();                                                  \
    using base_t::operator[];                                                  \
                                                                               \
    n() = default;                                                             \
    n(n const&) = default;                                                     \
    n(n&&) = default;                                                          \
    n& operator=(n const&) = default;                                          \
    n& operator=(n&&) = default;                                               \
    template <anypp::is_erased_data OTHER>                              \
    friend class anypp::any_base;                                       \
    template <anypp::is_any TO, anypp::is_any FROM>              \
    friend TO anypp::unchecked_downcast_to(FROM from)                   \
      requires(std::derived_from<TO, FROM>);                                   \
    template <anypp::is_erased_data OTHER>                              \
    using type_for =                                                           \
        n<_detail_ANYPP_TEMPLATE_ARGS(_add_head((OTHER), t))>;             \
  };

//    n(n&) = default;                                                           \

#define ANY_(n, BASE, l) ANY_TEMPLATE_((), n, BASE, l)

#define ANY(n, ...) ANY_(n, ::anypp::any_base, __VA_ARGS__)

#define ANY_TEMPLATE(t, n, l) \
  ANY_TEMPLATE_(t, n, ::anypp::any_base, l)

#define ANY_METHOD_(...) (__VA_ARGS__)

#define ANY_METHOD(ret, name, ...) \
  ANY_METHOD_(ret, name, name, false, , __VA_ARGS__)

#define ANY_CONST_METHOD(ret, name, ...) \
  ANY_METHOD_(ret, name, name, false, const, __VA_ARGS__)

#define ANY_OP(ret, op, ...)                                                  \
  ANY_METHOD_(ret, _detail_CONCAT(__op__, __COUNTER__), operator op, false, , \
             __VA_ARGS__)

#define ANY_OP_EXACT(ret, op, ...)                                           \
  ANY_METHOD_(ret, _detail_CONCAT(__op__, __COUNTER__), operator op, true, , \
             __VA_ARGS__)

#define ANY_CONST_OP(ret, op, ...)                                          \
  ANY_METHOD_(ret, _detail_CONCAT(__op__, __COUNTER__), operator op, false, \
             const, __VA_ARGS__)

#define ANY_CONST_OP_EXACT(ret, op, ...)                                   \
  ANY_METHOD_(ret, _detail_CONCAT(__op__, __COUNTER__), operator op, true, \
             const, __VA_ARGS__)

#define ANY_FORWARD(interface_namespace, interface_name, ...) \
  namespace interface_namespace {                                \
  template <_detail_ANYPP_TEMPLATE_FORMAL_ARGS(              \
      _add_head((ERASED_DATA), (__VA_ARGS__)))>                  \
  struct interface_name;                                         \
  struct interface_name##_v_table;                               \
  }

#define ANY_REGISTER_V_TABLE_INSTANCE(class_, interface_)                       \
  namespace {                                                                  \
  static auto __ =                                                             \
      anypp::bind_v_table_to_meta_data<interface_##_v_table, class_>(); \
  }

#ifdef ANY_DLL_MODE

#define ANY_V_TABLE_INSTANCE_FWD(export_, class_, interface_namespace_,       \
                                interface_)                                  \
  ANY_FORWARD(interface_namespace_, interface_)                           \
  namespace anypp {                                                   \
  template <>                                                                \
  export_ interface_namespace_::interface_##_v_table*                        \
  v_table_instance_implementaion<interface_namespace_::interface_##_v_table, \
                                 class_>();                                  \
  }

#define ANY_V_TABLE_INSTANCE(class_, interface_namespace_, interface_) \
  template <>                                                         \
  interface_namespace_::interface_##_v_table*                         \
  anypp::v_table_instance_implementaion<                       \
      interface_namespace_::interface_##_v_table, class_>() {         \
    static interface_namespace_::interface_##_v_table v_table{        \
        std::in_place_type<class_>};                                  \
    return &v_table;                                                  \
  }                                                                   \
  ANY_REGISTER_V_TABLE_INSTANCE(class_, interface_namespace_::interface_)

#else

#define ANY_V_TABLE_INSTANCE_FWD(...)
#define ANY_V_TABLE_INSTANCE(...)

#endif

#define ANY_V_TABLE_INSTANCE_STATIC(class_, interface_, interface_namespace_) \
  ANY_V_TABLE_INSTANCE_FWD(, class_, interface_, interface_namespace_)        \
  ANY_V_TABLE_INSTANCE(, class_, interface_, interface_namespace_)

#define ANY_V_TABLE_TEMPLATE_INSTANCE(export_, class, interface_, ...) \
  template <>                                                         \
  struct export_ interface_##_v_table_instance<class, __VA_ARGS__> {  \
    static interface_##_v_table<__VA_ARGS__>* get() {                 \
      static interface_##_v_table<__VA_ARGS__> v_table{               \
          std::in_place_type<class>};                                 \
      return &v_table;                                                \
    };                                                                \
  };                                                                  \
  namespace {                                                         \
  static auto __ = anypp::bind_v_table_to_meta_data<           \
      interface_##_v_table_instance<class, __VA_ARGS__>, class>();    \
  }

#define ANY_V_TABLE_HAS_EXTENSION_METHODS(interface_namespace, interface_name, \
                                         ...)                                 \
  ANY_FORWARD(interface_namespace, interface_name, __VA_ARGS__)            \
                                                                              \
  namespace anypp {                                                    \
  template <>                                                                 \
  constexpr bool has_extension_methods<interface_namespace::interface_name> = \
      true;                                                                   \
  }

#define ANY_NAME(...) __VA_ARGS__
