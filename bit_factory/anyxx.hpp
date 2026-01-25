#pragma once

//
// for Microsoft C++, you must enable the C-Preprocessor with this flag:
// /Zc:preprocessor (see CMakeLists.txt for example)
//

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstring>
#include <expected>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <variant>
#include <vector>

#if defined(__clang__)
#pragma GCC diagnostic ignored "-Wcast-function-type-mismatch"
#pragma GCC diagnostic ignored "-Wmicrosoft-template-shadow"
#pragma GCC diagnostic ignored "-Wunused-local-typedef"
#pragma GCC diagnostic ignored "-Wextra-semi"
#endif
#if defined(__GNUC__) and !defined(__clang__)
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

// --------------------------------------------------------------------------------
// any meta class, derived from this gem:
// https://github.com/AlexCodesApps/dynamic_interface

#define ANYXX_EXTRACT(...) ANYXX_EXTRACT __VA_ARGS__
#define ANYXX_NOTHING_ANYXX_EXTRACT
#define ANYXX_PASTE(x, ...) x##__VA_ARGS__
#define ANYXX_EVALUATING_PASTE(x, ...) ANYXX_PASTE(x, __VA_ARGS__)
#define ANYXX_UNPAREN(x) ANYXX_EVALUATING_PASTE(ANYXX_NOTHING_, ANYXX_EXTRACT x)
// usage:
static_assert(std::same_as<ANYXX_UNPAREN(int), int>);
static_assert(std::same_as<ANYXX_UNPAREN((int)), int>);

#define ANYXX_JACKET_RETURN(...) \
  anyxx::jacket_return<ANYXX_UNPAREN(ANYXX_UNPAREN(__VA_ARGS__))>

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
#define _detail_PARENS ()
#define _detail_APPLY(macro, args) macro args
#define _detail_REMOVE_PARENS(l) _detail_APPLY(_detail_EXPAND_1, l)
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

#define _detail_ANYXX_FORWARD_PARAM_LIST_H(b, c, f, ...)              \
  std::forward<decltype(c)>(c)                                        \
      __VA_OPT__(, _detail_ANYXX_FORWARD_PARAM_LIST_A _detail_PARENS( \
                       b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_ANYXX_FORWARD_PARAM_LIST_A() _detail_ANYXX_FORWARD_PARAM_LIST_H
#define _detail_ANYXX_FORWARD_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_ANYXX_FORWARD_PARAM_LIST_H(__VA_ARGS__))

#define _detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP_H(b, c, param_type, ...)      \
  anyxx::v_table_to_map<Concrete, ANYXX_UNPAREN(param_type)>::                \
      template forward<decltype(c)>(std::forward<decltype(c)>(c)) __VA_OPT__( \
          , _detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP_A _detail_PARENS(         \
                b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP_A() \
  _detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP_H
#define _detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP(...) \
  _detail_EXPAND_(_detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP_H(__VA_ARGS__))

#define _detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP_H(b, c, param_type,    \
                                                         ...)                 \
  anyxx::forward_trait_to_map<traited_t, ANYXX_UNPAREN(param_type)>::         \
      template forward<decltype(c)>(std::forward<decltype(c)>(c)) __VA_OPT__( \
          , _detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP_A _detail_PARENS(  \
                b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP_A() \
  _detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP_H
#define _detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP(...) \
  _detail_EXPAND_(_detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP_H(__VA_ARGS__))

#define _detail_ANYXX_JACKET_PARAM_LIST_H(b, c, param_type, ...) \
  [[maybe_unused]] auto&& c __VA_OPT__(                          \
      , _detail_ANYXX_JACKET_PARAM_LIST_A _detail_PARENS(        \
            b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_ANYXX_JACKET_PARAM_LIST_A() _detail_ANYXX_JACKET_PARAM_LIST_H
#define _detail_ANYXX_JACKET_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_ANYXX_JACKET_PARAM_LIST_H(__VA_ARGS__))
#define _detail_EXPAND_LIST(...) __VA_ARGS__

#define _detail_ANYXX_V_TABLE_PARAM_LIST_H(b, c, param_type, ...)    \
  [[maybe_unused]] anyxx::v_table_param<any_value_t,                 \
                                        ANYXX_UNPAREN(param_type)> c \
  __VA_OPT__(, _detail_ANYXX_V_TABLE_PARAM_LIST_A _detail_PARENS(    \
                   b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_ANYXX_V_TABLE_PARAM_LIST_A() _detail_ANYXX_V_TABLE_PARAM_LIST_H
#define _detail_ANYXX_V_TABLE_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_ANYXX_V_TABLE_PARAM_LIST_H(__VA_ARGS__))
#define _detail_EXPAND_LIST(...) __VA_ARGS__

#define _detail_ANYXX_MAP_PARAM_LIST_H(b, c, param_type, ...)                  \
  [[maybe_unused]] auto&& c __VA_OPT__(                                        \
      , _detail_ANYXX_MAP_PARAM_LIST_A _detail_PARENS(b, _detail_CONCAT(b, c), \
                                                      __VA_ARGS__))
#define _detail_ANYXX_MAP_PARAM_LIST_A() _detail_ANYXX_MAP_PARAM_LIST_H
#define _detail_ANYXX_MAP_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_ANYXX_MAP_PARAM_LIST_H(__VA_ARGS__))
#define _detail_EXPAND_LIST(...) __VA_ARGS__

#define _detail_ANYXX_TYPENAME_PARAM_H(t) _detail_ANYXX_TYPENAME_PARAM t
#define _detail_ANYXX_TYPENAME_PARAM(t) , typename t
#define _detail_ANYXX_TYPENAME_PARAM_LIST(head, ...) \
  typename _detail_REMOVE_PARENS(head) __VA_OPT__(   \
      _detail_foreach_macro(_detail_ANYXX_TYPENAME_PARAM_H, __VA_ARGS__))
#define _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(...) \
  __VA_OPT__(template <_detail_ANYXX_TYPENAME_PARAM_LIST(__VA_ARGS__)>)

#define _detail_ANYXX_TEMPLATE_ARG_H(t) _detail_ANYXX_TEMPLATE_ARG t
#define _detail_ANYXX_TEMPLATE_ARG(t) , t
#define _detail_ANYXX_TEMPLATE_ARGS1(head, ...) \
  _detail_REMOVE_PARENS(head) __VA_OPT__(       \
      _detail_foreach_macro(_detail_ANYXX_TEMPLATE_ARG_H, __VA_ARGS__))
#define _detail_ANYXX_TEMPLATE_ARGS(...) \
  __VA_OPT__(_detail_ANYXX_TEMPLATE_ARGS1(__VA_ARGS__))
#define _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(...) \
  __VA_OPT__(<_detail_ANYXX_TEMPLATE_ARGS(__VA_ARGS__)>)

#define _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS_H(...) \
  __VA_OPT__(<_detail_ANYXX_TEMPLATE_ARGS(__VA_ARGS__), anyxx::dyn>)
#define _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS(t) \
  _detail_ANYXX_V_TABLE_TEMPLATE_FORMAL_ARGS_H t

#define _detail_LEAD_COMMA_H(...) __VA_OPT__(, )
#define _detail_ANYXX_FPD_H(l) _detail_ANYXX_FUNCTION_PTR_DECL l
#define _detail_ANYXX_MEMEBER_LIMP_H(l) _detail_ANYXX_LAMBDA_TO_MEMEBER_IMPL l
#define _detail_ANYXX_FN_H(l) _detail_ANYXX_FN l

#define _detail_LEAD_COMMA_H_E(l) _detail_LEAD_COMMA_H l

#define __detail_ANYXX_ADD_HEAD(h, ...) h __VA_OPT__(, ) __VA_ARGS__
#define __detail_ANYXX_ADD_HEAD_LIST(l, ...) \
  __detail_ANYXX_ADD_HEAD(_detail_REMOVE_PARENS(l), __VA_ARGS__)
// Examples:
// __detail_ANYXX_ADD_HEAD(H, A, B, C, D)  -> H, A, B, C, D
// __detail_ANYXX_ADD_HEAD((H), (A), (B), (C), (D)) -> (H), (A), (B), (C), (D)
// __detail_ANYXX_ADD_HEAD_LIST(((H1),(H2)), (A), (B), (C), (D))
//  -> (H1), (H2), (A), (B), (C), (D)

#define __detail_ANYXX_ADD_TAIL(t, ...) __VA_ARGS__ __VA_OPT__(, ) t
// Examples:
// __detail_ANYXX_ADD_TAIL(T, A, B, C, D) -> (A), (B), (C), (D), (T)
// __detail_ANYXX_ADD_TAIL(T, A, B, C, D) -> (A), (B), (C), (D), (T)

#define _typename _typename1
#define _typename1(t) t

#define _detail_ANYXX_TEMPLATE_FORMAL_ARG_H(l) \
  _detail_ANYXX_TEMPLATE_FORMAL_ARG l
#define _detail_ANYXX_TEMPLATE_FORMAL_ARG(_typename) , typename _typename
#define _detail_ANYXX_TEMPLATE_FORMAL_ARGS(...) \
  __VA_OPT__(_detail_ANYXX_TEMPLATE_FORMAL_ARGS1(__VA_ARGS__))
#define _detail_ANYXX_TEMPLATE_FORMAL_ARGS1(h, ...) \
  typename _typename h __VA_OPT__(                  \
      _detail_ANYXX_TEMPLATE_FORMAL_ARGS2((__VA_ARGS__)))
#define _detail_ANYXX_TEMPLATE_FORMAL_ARGS2(l)               \
  _detail_foreach_macro(_detail_ANYXX_TEMPLATE_FORMAL_ARG_H, \
                        _detail_EXPAND_LIST l)

#define _detail_ANYXX_V_TABLE_TEMPLATE_HEADER_H(...) \
  __VA_OPT__(template <_detail_ANYXX_TEMPLATE_FORMAL_ARGS(__VA_ARGS__)>)

#define _detail_ANYXX_V_TABLE_TEMPLATE_HEADER(t) \
  _detail_ANYXX_V_TABLE_TEMPLATE_HEADER_H t

#define _detail_ANYXX_INVOKE_TEMPLATE_PARAMS_H(...) __VA_OPT__(<__VA_ARGS__>)

#define _detail_ANYXX_INVOKE_TEMPLATE_PARAMS(t) \
  _detail_ANYXX_INVOKE_TEMPLATE_PARAMS_H t

#define _detail_ANYXX_EXPAND_WITH_LEADING_COMMA(...) __VA_OPT__(, ) __VA_ARGS__

#define _detail_ANYXX_OPTIONAL_TEMPLATE(...) __VA_OPT__(template)

#define _detail_ANYXX_MAP_LIMP_H(l) _detail_ANYXX_MAP_IMPL l
#define _detail_ANYXX_MAP_IMPL(overload, type, name, name_ext, exact_const,  \
                               const_, trait_body, ...)                      \
  static auto name([[maybe_unused]] T const_& x __VA_OPT__(                  \
      , _detail_ANYXX_MAP_PARAM_LIST_H(a, _sig, __VA_ARGS__)))               \
      -> anyxx::map_return<T, ANYXX_UNPAREN(type)> {                         \
    return _detail_REMOVE_PARENS(trait_body)(                                \
        __VA_OPT__(_detail_ANYXX_FORWARD_PARAM_LIST(a, _sig, __VA_ARGS__))); \
  };

#define _detail_ANYXX_MAP_VARIANT_LIMP_H(l) _detail_ANYXX_MAP_VARIANT_IMPL l
#define _detail_ANYXX_MAP_VARIANT_IMPL(overload, type, name, name_ext,       \
                                       exact_const, const_, trait_body, ...) \
  static auto name([[maybe_unused]] T const_& x __VA_OPT__(                  \
      , _detail_ANYXX_MAP_PARAM_LIST_H(a, _sig, __VA_ARGS__)))               \
      -> decltype(auto) {                                                    \
    return std::visit(                                                       \
        anyxx::overloads{                                                    \
            [&]<typename V>(V&& v) {                                         \
              return x_model_map<std::decay_t<V>>::name(                     \
                  std::forward<V>(v) __VA_OPT__(, )                          \
                      __VA_OPT__(_detail_ANYXX_FORWARD_PARAM_LIST(           \
                          a, _sig, __VA_ARGS__)));                           \
            },                                                               \
            [&]<anyxx::is_any Any>([[maybe_unused]] Any&& any) {             \
              return std::forward<Any>(any).name(__VA_OPT__(                 \
                  _detail_ANYXX_FORWARD_PARAM_LIST(a, _sig, __VA_ARGS__)));  \
            }},                                                              \
        x);                                                                  \
  };

#define _detail_ANYXX_FUNCTION_PTR_DECL(overload, type, name, name_ext,     \
                                        exact_const, const_, map_body, ...) \
  anyxx::v_table_return<any_value_t, ANYXX_UNPAREN(type)> (*name)(          \
      void const_* __VA_OPT__(                                              \
          , _detail_ANYXX_V_TABLE_PARAM_LIST(a, _sig, __VA_ARGS__)));

#define _detail_ANYXX_LAMBDA_TO_MEMEBER_IMPL(                           \
    overload, type, name, name_ext, exact_const, const_, map_body, ...) \
  name = [](void const_* _vp __VA_OPT__(                                \
             , _detail_ANYXX_V_TABLE_PARAM_LIST(a, _sig, __VA_ARGS__))) \
      -> anyxx::v_table_return<any_value_t, ANYXX_UNPAREN(type)> {      \
    if constexpr (std::same_as<anyxx::self&, ANYXX_UNPAREN(type)>) {    \
      concept_map{}.name(                                               \
          *anyxx::unchecked_unerase_cast<Concrete>(_vp) __VA_OPT__(, )  \
              __VA_OPT__(_detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP(       \
                  a, _sig, __VA_ARGS__)));                              \
      return anyxx::handle_self_ref_return<ANYXX_UNPAREN(type)>{}();    \
    } else {                                                            \
      return concept_map{}.name(                                        \
          *anyxx::unchecked_unerase_cast<Concrete>(_vp) __VA_OPT__(, )  \
              __VA_OPT__(_detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP(       \
                  a, _sig, __VA_ARGS__)));                              \
    }                                                                   \
  };

#define _detail_ANYXX_FN(overload, type, name, name_ext, exact_const, const_,  \
                         map_body, ...)                                        \
  overload template <typename Self>                                            \
  decltype(auto) name_ext(this Self&& self __VA_OPT__(, ) __VA_OPT__(          \
      _detail_ANYXX_JACKET_PARAM_LIST(a, _sig, __VA_ARGS__)))                  \
    requires(::anyxx::const_correct_call_for_proxy<                            \
             void const_*, typename std::decay_t<Self>::proxy_t, exact_const>) \
  {                                                                            \
    using self_t = std::decay_t<Self>;                                         \
    using T = typename self_t::T;                                              \
    using proxy_t = typename self_t::proxy_t;                                  \
                                                                               \
    if constexpr (!self_t::dyn) {                                              \
      using traited_t = typename proxy_t::value_t;                             \
      if constexpr (std::same_as<void, ANYXX_UNPAREN(type)>) {                 \
        return static_dispatch_map_t<T>::name(                                 \
            get_proxy_value(self) __VA_OPT__(, )                               \
                __VA_OPT__(_detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP(     \
                    a, _sig, __VA_ARGS__)));                                   \
      } else {                                                                 \
        return ANYXX_JACKET_RETURN(type)::forward(                             \
            static_dispatch_map_t<T>::name(                                    \
                get_proxy_value(self) __VA_OPT__(, )                           \
                    __VA_OPT__(_detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP( \
                        a, _sig, __VA_ARGS__))),                               \
            std::forward<Self>(self));                                         \
      }                                                                        \
    } else {                                                                   \
      if constexpr (std::same_as<void, ANYXX_UNPAREN(type)>) {                 \
        return get_v_table(self)->name(anyxx::get_proxy_ptr(self) __VA_OPT__(  \
            , _detail_ANYXX_FORWARD_PARAM_LIST(a, _sig, __VA_ARGS__)));        \
      } else {                                                                 \
        return ANYXX_JACKET_RETURN(type)::forward(                             \
            get_v_table(self)->name(anyxx::get_proxy_ptr(self) __VA_OPT__(     \
                , _detail_ANYXX_FORWARD_PARAM_LIST(a, _sig, __VA_ARGS__))),    \
            std::forward<Self>(self));                                         \
      }                                                                        \
    }                                                                          \
  }

#define _detail_ANYXX_MAP_FUNCTIONS(...)                     \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MAP_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_MAP_VARIANT_FUNCTIONS(...)                     \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MAP_VARIANT_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_V_TABLE_FUNCTION_PTRS(...)        \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_FPD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYXX_V_TABLE_LAMBDAS(...)                       \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MEMEBER_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYXX_FNS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_FN_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_MAKE_V_TABLE_FUNCTION_NAME(n) \
  _detail_CONCAT(make_, _detail_CONCAT(n, _v_table))

// cppcheck-suppress-macro performance-unnecessary-value-param
#define TRAIT_META_FUNCTION(                                                   \
    any_template_params, model_map_template_params, concrete_template_params,  \
    static_dispatch_template_params, variant_model_map_template_params, n,     \
    BASE, base_template_params, l, decoration)                                 \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(any_template_params) struct n;    \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
  struct n##_default_model_map {                                               \
    _detail_ANYXX_MAP_FUNCTIONS(l)                                             \
  };                                                                           \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
  struct n##_model_map : n##_default_model_map<_detail_ANYXX_TEMPLATE_ARGS(    \
                             model_map_template_params)> {};                   \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
    requires(anyxx::is_variant<T>)                                             \
  struct n##                                                                   \
      _model_map<_detail_ANYXX_TEMPLATE_ARGS(model_map_template_params)> {     \
    template <typename V>                                                      \
    using x_model_map = n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(             \
        variant_model_map_template_params)>;                                   \
    _detail_ANYXX_MAP_VARIANT_FUNCTIONS(l)                                     \
  };                                                                           \
                                                                               \
  struct n##_has_open_dispatch;                                                \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(                                  \
      any_template_params) struct n##_v_table                                  \
      : BASE                                                                   \
        _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(base_template_params)::v_table_t, \
        anyxx::dispatch_holder<anyxx::is_type_complete<n##_has_open_dispatch>, \
                               n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(         \
                                   any_template_params)> {                     \
    using v_table_base_t = typename BASE _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS( \
        base_template_params)::v_table_t;                                      \
                                                                               \
    using v_table_t = n##_v_table;                                             \
                                                                               \
    using any_value_t =                                                        \
        anyxx::any<anyxx::val, n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(         \
                                   any_template_params)>;                      \
                                                                               \
    static constexpr bool open_dispatch_enabeled =                             \
        anyxx::is_type_complete<n##_has_open_dispatch>;                        \
    using own_dispatch_holder_t = typename anyxx::dispatch_holder<             \
        open_dispatch_enabeled,                                                \
        n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(any_template_params)>;          \
                                                                               \
    static bool static_is_derived_from(const std::type_info& from) {           \
      return typeid(v_table_t) == from                                         \
                 ? true                                                        \
                 : v_table_base_t::static_is_derived_from(from);               \
    }                                                                          \
                                                                               \
    _detail_ANYXX_V_TABLE_FUNCTION_PTRS(l);                                    \
                                                                               \
    template <typename Concrete>                                               \
    explicit(false) n##_v_table(std::in_place_type_t<Concrete> concrete);      \
  };                                                                           \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(any_template_params) struct n     \
      : BASE                                                                   \
        _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(base_template_params) {           \
    using any_value_t =                                                        \
        anyxx::any<anyxx::val, n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(         \
                                   any_template_params)>;                      \
                                                                               \
    using base_t =                                                             \
        BASE _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(base_template_params);       \
                                                                               \
    using v_table_base_t = base_t::v_table_t;                                  \
    using v_table_t =                                                          \
        n##_v_table _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(any_template_params); \
    template <typename StaticDispatchType>                                     \
    using static_dispatch_map_t = n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(   \
        static_dispatch_template_params)>;                                     \
                                                                               \
    _detail_ANYXX_FNS(l)                                                       \
                                                                               \
        _detail_REMOVE_PARENS(decoration)                                      \
  };                                                                           \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(                                  \
      any_template_params) template <typename Concrete>                        \
  n##_v_table                                                                  \
  _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(any_template_params)::n##_v_table(      \
      std::in_place_type_t<Concrete> concrete)                                 \
      : v_table_base_t(concrete) {                                             \
    using concept_map =                                                        \
        n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(concrete_template_params)>;  \
                                                                               \
    _detail_ANYXX_V_TABLE_LAMBDAS(l);                                          \
                                                                               \
    if constexpr (open_dispatch_enabeled) {                                    \
      own_dispatch_holder_t::set_dispatch_table(                               \
          ::anyxx::dispatch_table_instance<n##_v_table, Concrete>());          \
    }                                                                          \
                                                                               \
    ::anyxx::set_is_derived_from<v_table_t>(this);                             \
  };

#define __detail_ANYXX_TRAIT_(t, n, BASE, l, decoration)                       \
  TRAIT_META_FUNCTION(, (T), (Concrete), (StaticDispatchType), (V), n, BASE, , \
                      l, decoration)

#define TRAIT_EX_(n, BASE, l, decoration) \
  __detail_ANYXX_TRAIT_(, n, BASE, l, decoration)

#define TRAIT_(n, BASE, l) TRAIT_EX_(n, BASE, l, ())

#define TRAIT(n, fns) TRAIT_(n, anyxx::base_trait, fns)
#define TRAIT_EX(n, ...) TRAIT_EX_(n, anyxx::base_trait, __VA_ARGS__)

#define TRAIT_TEMPLATE_EX_(t, n, base, base_template_types, l, decoration)     \
  TRAIT_META_FUNCTION(                                                         \
      _detail_REMOVE_PARENS(t),                                                \
      __detail_ANYXX_ADD_HEAD((T), _detail_REMOVE_PARENS(t)),                  \
      __detail_ANYXX_ADD_HEAD((Concrete), _detail_REMOVE_PARENS(t)),           \
      __detail_ANYXX_ADD_HEAD((StaticDispatchType), _detail_REMOVE_PARENS(t)), \
      __detail_ANYXX_ADD_HEAD((V), _detail_REMOVE_PARENS(t)), n, base,         \
      _detail_REMOVE_PARENS(base_template_types), l, decoration)

#define TRAIT_TEMPLATE_EX(t, n, l, decoration) \
  TRAIT_TEMPLATE_EX_(t, n, anyxx::base_trait, (), l, decoration)

#define TRAIT_TEMPLATE_(t, n, base, base_template_types, l) \
  TRAIT_TEMPLATE_EX_(t, n, base, base_template_types, l, ())

#define TRAIT_TEMPLATE(t, n, l) TRAIT_TEMPLATE_(t, n, anyxx::base_trait, (), l)

////////////////////////////////////////////////////////////////////////////////
// cppcheck-suppress-macro performance-unnecessary-value-param
#define ANY_META_FUNCTION(pure_template_params,                             \
                          any_template_params_with_defaults, n)             \
                                                                            \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(                              \
      any_template_params_with_defaults)>                                   \
  using any_##n = anyxx::any<Proxy, n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS( \
                                        pure_template_params)>;

////////////////////////////////////////////////////////////////////////////////

#define __detail_ANYXX_ANY_CMF(t, t_with_defaults, n) \
  ANY_META_FUNCTION(, _detail_REMOVE_PARENS(t_with_defaults), n)

#define __detail_ANYXX_ANY_EX_(n, proxy_default) \
  __detail_ANYXX_ANY_CMF(                        \
      ((Proxy)), ((Proxy = anyxx::default_proxy<proxy_default>::type)), n)

#define ANY_EX_(n, BASE, l, proxy_default, decoration) \
  TRAIT_EX_(n, BASE, l, decoration)                    \
  __detail_ANYXX_ANY_EX_(n, proxy_default)

#define ANY_EX(n, l, proxy_default, decoration) \
  TRAIT_EX(n, l, decoration)                    \
  __detail_ANYXX_ANY_EX_(n, proxy_default)

#define ANY_(n, BASE, l, proxy_default) \
  TRAIT_(n, BASE, l)                    \
  __detail_ANYXX_ANY_EX_(n, proxy_default)

#define ANY(n, l, ...) \
  TRAIT(n, l)          \
  __detail_ANYXX_ANY_EX_(n, __VA_ARGS__)

#define __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)                 \
  ANY_META_FUNCTION(_detail_REMOVE_PARENS(t),                                \
                    __detail_ANYXX_ADD_TAIL(                                 \
                        (Proxy = anyxx::default_proxy<proxy_default>::type), \
                        _detail_REMOVE_PARENS(t)),                           \
                    n)

#define ANY_TEMPLATE_EX_(t, n, BASE, bt, l, proxy_default, decoration) \
  TRAIT_TEMPLATE_EX_(t, n, BASE, bt, l, decoration)                    \
  __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)

#define ANY_TEMPLATE_(t, n, BASE, bt, l, proxy_default) \
  TRAIT_TEMPLATE_(t, n, BASE, bt, l)                    \
  __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)

#define ANY_TEMPLATE(t, n, l, proxy_default) \
  TRAIT_TEMPLATE(t, n, l)                    \
  __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)

#define ANY_TEMPLATE_EX(t, n, l, proxy_default, decoration) \
  TRAIT_TEMPLATE_EX(t, n, l, decoration)                    \
  __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)

#define ANY_FN_(...) (__VA_ARGS__)
#define ANY_OVERLOAD(name) using base_t::name;

#define __detail_ANYXX_MEMBER_FN(overload, ret, name, name_ext, exact_const, \
                                 const_, params)                             \
  ANY_FN_(overload, ret, name, name_ext, exact_const, const_, (x.name_ext),  \
          _detail_EXPAND params)

#define ANY_FN_PURE(ret, name, params, const_)            \
  ANY_FN_(, ret, name, name, false, const_,               \
          (_detail_ANYXX_TRAIT_ERROR_MESSAGE(name, ret)), \
          _detail_EXPAND params)
#define ANY_FN_PURE_EXACT(ret, name, params, const_)      \
  ANY_FN_(, ret, name, name, true, const_,                \
          (_detail_ANYXX_TRAIT_ERROR_MESSAGE(name, ret)), \
          _detail_EXPAND params)
#define ANY_FN_DEF(ret, name, params, const_, ...)         \
  ANY_FN_(, ret, name, name, false, const_, (__VA_ARGS__), \
          _detail_EXPAND params)
#define ANY_FN_DEF_EXACT(ret, name, params, const_, ...) \
  ANY_FN_(, ret, name, name, true, const_, (__VA_ARGS__), _detail_EXPAND params)
#define ANY_FN(ret, name, params, const_) \
  __detail_ANYXX_MEMBER_FN(, ret, name, name, false, const_, params)
#define ANY_FN_EXACT(ret, name, params, const_) \
  __detail_ANYXX_MEMBER_FN(, ret, name, name, true, const_, params)
#define ANY_FN_OVERLOAD(ret, name, params, const_)                             \
  __detail_ANYXX_MEMBER_FN(ANY_OVERLOAD(name), ret, name, name, false, const_, \
                           params)
#define ANY_FN_OVERLOAD_EXACT(ret, name, params, const_)                      \
  __detail_ANYXX_MEMBER_FN(ANY_OVERLOAD(name), ret, name, name, true, const_, \
                           params)

#define ANY_OP_MAP_NAMED(ret, op, name, params, const_) \
  __detail_ANYXX_MEMBER_FN(, ret, name, operator op, false, const_, params)
#define ANY_OP(ret, op, params, const_) \
  ANY_OP_MAP_NAMED(ret, op, _detail_CONCAT(__op__, __COUNTER__), params, const_)
#define ANY_OP_DEF(ret, op, name, params, const_, ...)            \
  ANY_FN_(, ret, name, operator op, false, const_, (__VA_ARGS__), \
          _detail_EXPAND params)

#define ANY_OP_EXACT_MAP_NAMED(ret, op, name, params, const_) \
  __detail_ANYXX_MEMBER_FN(, ret, name, operator op, true, const_, params)
#define ANY_OP_EXACT(ret, op, params, const_)                                  \
  ANY_OP_EXACT_MAP_NAMED(ret, op, _detail_CONCAT(__op__, __COUNTER__), params, \
                         const_)
#define ANY_OP_EXACT_DEF(ret, op, name, params, const_, ...)     \
  ANY_FN_(, ret, name, operator op, true, const_, (__VA_ARGS__), \
          _detail_EXPAND params)

#define ANY_OP_EXACT_OVERLOAD_MAP_NAMED(ret, op, name, params, const_)        \
  __detail_ANYXX_MEMBER_FN(ANY_OVERLOAD(operator op), ret, name, operator op, \
                           true, const_, params)
#define ANY_OP_EXACT_OVERLOAD(ret, op, params, const_) \
  ANY_OP_EXACT_OVERLOAD_MAP_NAMED(                     \
      ret, op, _detail_CONCAT(__op__, __COUNTER__), params, const_)
#define ANY_OP_EXACT_OVERLOAD_DEF(ret, op, name, params, const_, ...)      \
  ANY_FN_(ANY_OVERLOAD(operator op), ret, name, operator op, true, const_, \
          (__VA_ARGS__), _detail_EXPAND params)

#define __ANY_MODEL_MAP(class_, interface_, t)                  \
  template <>                                                   \
  struct interface_##_model_map<_detail_ANYXX_TEMPLATE_ARGS(t)> \
      : interface_##_default_model_map<_detail_ANYXX_TEMPLATE_ARGS(t)>

#define ANY_TEMPLATE_MODEL_MAP(class_, interface_, t) \
  __ANY_MODEL_MAP(class_, interface_,                 \
                  __detail_ANYXX_ADD_HEAD(class_, _detail_REMOVE_PARENS(t)))

#define ANY_MODEL_MAP(class_, interface_) \
  __ANY_MODEL_MAP(class_, interface_, class_)

#define _detail_ANYXX_TRAIT_ERROR_MESSAGE(name, ret)     \
[]<typename... Args>([[maybe_unused]](Args...) -> ret{ \
  static_assert(                                      \
      anyxx::missing_trait_error<T>::not_specialized, \
      "'" #name                                       \
      "' is missing in the specialization of this proxy_trait!"); \
return {};\
}

namespace anyxx {

template <typename, typename = void>
struct is_type_complete_impl : std::false_type {};
template <typename T>
struct is_type_complete_impl<
    T, std::enable_if_t<std::is_object<T>::value &&
                        !std::is_pointer<T>::value && (sizeof(T) > 0)>>
    : std::true_type {};
template <typename T>
constexpr static inline bool is_type_complete = is_type_complete_impl<T>::value;

template <class... Ts>
struct overloads : Ts... {
  using Ts::operator()...;
};

template <typename T>
struct is_variant_impl : std::false_type {};
template <typename... Args>
struct is_variant_impl<std::variant<Args...>> : std::true_type {};
template <typename T>
inline constexpr bool is_variant = is_variant_impl<T>::value;

#ifdef ANY_DLL_MODE
constexpr bool is_in_dll_mode = true;
#else
constexpr bool is_in_dll_mode = false;
#endif

class error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};
class type_mismatch_error : public error {
  using error::error;
};

struct member_dispatch {};
struct dyn : member_dispatch {};
struct static_ : member_dispatch {};

template <typename Dispatch>
concept is_member_dispatch = std::derived_from<Dispatch, member_dispatch>;
static_assert(is_member_dispatch<dyn>);
static_assert(is_member_dispatch<static_>);

template <typename T>
struct missing_trait_error {
  static constexpr bool not_specialized = false;
};
template <typename Value>
struct by_val;

using const_void = void const*;
using mutable_void = void*;
template <typename V>
concept voidness =
    (std::same_as<V, const_void> || std::same_as<V, mutable_void>);
template <voidness Voidness>
struct is_const_void_;
template <>
struct is_const_void_<void*> : std::false_type {};
template <>
struct is_const_void_<void const*> : std::true_type {};
template <typename Voidness>
concept is_const_void = is_const_void_<Voidness>::value;

class meta_data;

template <typename Model>
constexpr inline std::size_t compute_model_size() {
  if constexpr (std::is_trivially_copyable_v<Model>) {
    return 0;
  } else {
    return sizeof(Model);
  }
}

struct any_v_table;

template <bool HasDispatch, typename Trait>
struct dispatch_holder;
using dispatch_table_function_t = void (*)();
using dispatch_table_dispatch_index_t = std::size_t;
using dispatch_table_entry_t = unsigned long long;
using dispatch_table_t = std::vector<dispatch_table_entry_t>;
template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance_implementation() {
  static dispatch_table_t dispatch_table;
  return &dispatch_table;
}
#ifdef ANY_DLL_MODE
template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance();
#else
template <typename AnyVTable, typename Class>
dispatch_table_t* dispatch_table_instance() {
  return dispatch_table_instance_implementation<AnyVTable, Class>();
}
#endif

template <typename VTable, typename Concrete>
VTable* v_table_instance();

struct any_v_table {
  template <typename Concrete>
  explicit any_v_table([[maybe_unused]] std::in_place_type_t<Concrete> concrete)
      : model_size(compute_model_size<Concrete>()),
        allocate(+[] -> mutable_void {
          return std::allocator<Concrete>{}.allocate(1);
        }),
        copy_constructor(+[]([[maybe_unused]] mutable_void placement,
                             [[maybe_unused]] const_void from) -> mutable_void {
          if constexpr (std::is_copy_constructible_v<Concrete>) {
            return std::construct_at<Concrete>(
                static_cast<Concrete*>(placement),
                *static_cast<Concrete const*>(from));
          } else {
            return nullptr;
          };
        }),
        move_constructor(
            +[]([[maybe_unused]] mutable_void placement,
                [[maybe_unused]] mutable_void from) -> mutable_void {
              if constexpr (std::is_move_constructible_v<Concrete>) {
                return std::construct_at<Concrete>(
                    static_cast<Concrete*>(placement),
                    std::move(*static_cast<Concrete*>(from)));
              } else {
                return nullptr;
              };
            }),
        destructor(+[](mutable_void data) noexcept -> void {
          std::destroy_at(static_cast<Concrete*>(data));
        }),
        delete_(+[](mutable_void data) noexcept -> void {
          if (!data) return;
          auto p = static_cast<Concrete*>(data);
          std::destroy_at(p);
          std::allocator<Concrete>{}.deallocate(p, 1);
        }),
        get_type_info(+[]() noexcept -> std::type_info const& {
          return typeid(Concrete);
        }),
        is_derived_from_(+[](const std::type_info& from) {
          return static_is_derived_from(from);
        }) {}

  std::size_t model_size = 0u;
  mutable_void (*allocate)();
  mutable_void (*copy_constructor)(mutable_void placement, const_void from);
  mutable_void (*move_constructor)(mutable_void placement, mutable_void from);
  void (*destructor)(mutable_void data) noexcept;
  void (*delete_)(mutable_void) noexcept;
  std::type_info const& (*get_type_info)() noexcept;
  bool (*is_derived_from_)(const std::type_info&);

  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(any_v_table) == from;
  }

  meta_data* meta_data_ = nullptr;
};

inline bool is_derived_from(const std::type_info& from,
                            any_v_table const* v_table) {
  return v_table->is_derived_from_(from);
}

inline std::size_t model_size(any_v_table* v_table) {
  return v_table ? v_table->model_size : 0u;
}
inline mutable_void copy_construct_at(any_v_table* v_table,
                                      mutable_void placement, const_void from) {
  return v_table->copy_constructor(placement, from);
}
inline mutable_void copy_construct(any_v_table* v_table, const_void from) {
  return copy_construct_at(v_table, v_table->allocate(), from);
}
inline mutable_void move_construct_at(any_v_table* v_table,
                                      mutable_void placement,
                                      mutable_void from) {
  return v_table->move_constructor(placement, from);
}
inline mutable_void move_construct(any_v_table* v_table, mutable_void from) {
  return move_construct_at(v_table, v_table->allocate(), from);
}
inline void delete_(any_v_table* v_table, mutable_void& data) noexcept {
  if (!data) return;
  assert(v_table);
  v_table->delete_(data);
  data = nullptr;
}  // NOLINT

template <typename U>
bool type_match(any_v_table* v_table);

template <typename U>
void check_type_match(any_v_table* v_table) {
  if (!type_match<U>(v_table)) throw type_mismatch_error("type mismatch");
}

template <typename Data>
struct proxy_trait;

template <typename Proxy>
struct basic_proxy_trait {
  inline static constexpr bool is_weak = false;

  static void move_to(auto& to, [[maybe_unused]] auto, auto&& from,
                      [[maybe_unused]] auto) {
    to = std::move(from);
  }

  static void copy_construct_from(Proxy& to, [[maybe_unused]] any_v_table*,
                                  auto const& from, [[maybe_unused]] auto) {
    to = from;
  }

  static void destroy([[maybe_unused]] Proxy const& data,
                      [[maybe_unused]] any_v_table* v_table) {}
};

template <typename E>
concept is_proxy = requires(E e, mutable_void void_data, any_v_table* v_table) {
  typename proxy_trait<E>::void_t;
  typename proxy_trait<E>::static_dispatch_t;
  { proxy_trait<E>::is_constructibile_from_const } -> std::convertible_to<bool>;
  { proxy_trait<E>::is_owner } -> std::convertible_to<bool>;
  {
    proxy_trait<E>::get_proxy_ptr_in(e, v_table)
  } -> std::convertible_to<typename proxy_trait<E>::void_t>;
  { proxy_trait<E>::is_weak } -> std::convertible_to<bool>;
  { proxy_trait<E>::clone_from(void_data, v_table) };
};

using emtpty_trait_v_table = any_v_table;
struct base_trait {
  using v_table_t = emtpty_trait_v_table;
};

template <is_proxy Proxy, typename Trait = base_trait>
class any;

template <typename Proxy>
concept is_dyn =
    is_proxy<Proxy> && voidness<typename proxy_trait<Proxy>::static_dispatch_t>;

template <typename I>
concept is_proxy_holder_impl = requires(I i) {
  typename I::proxy_t;
  typename I::proxy_trait_t;
};
template <typename I>
concept is_proxy_holder = is_proxy_holder_impl<std::decay_t<I>>;

template <typename I>
concept is_any_impl = is_proxy_holder_impl<I> &&
                      requires(I::proxy_t ed) { typename I::v_table_t; };
template <typename I>
concept is_any = is_any_impl<std::decay_t<I>>;

template <class E>
concept is_typed_any = is_any<E> && requires(E e) {
  typename E::proxy_trait_t;
  typename E::value_t;
  { E::is_const } -> std::convertible_to<bool>;
};

template <typename ConstructedWith, typename Proxy, typename BASE>
concept erased_constructibile_for =
    (!std::derived_from<std::remove_cvref_t<ConstructedWith>, BASE> &&
     !is_proxy<std::remove_cvref_t<ConstructedWith>> &&
     (!std::is_const_v<std::remove_reference_t<ConstructedWith>> ||
      proxy_trait<Proxy>::is_constructibile_from_const));

template <typename ConstructedWith, typename Proxy>
concept constructibile_for =
    (proxy_trait<Proxy>::template is_constructibile_from<
        ConstructedWith>::value) ||
    (erased_constructibile_for<ConstructedWith, Proxy, any<Proxy>> &&
     !is_proxy_holder<ConstructedWith> &&
     !is_typed_any<std::remove_cvref_t<ConstructedWith>>);

template <is_proxy Data>
using data_void = proxy_trait<Data>::void_t;

template <typename Proxy>
concept is_const_data = is_proxy<Proxy> && is_const_void<data_void<Proxy>>;

template <typename Proxy>
concept is_weak_data = is_proxy<Proxy> && proxy_trait<Proxy>::is_weak;

template <bool CallIsConst, bool ErasedDataIsConst, bool ErasedDataIsWeak>
concept const_correct_call =
    !ErasedDataIsWeak &&
    ((CallIsConst == ErasedDataIsConst) || !ErasedDataIsConst);

template <typename CALL, typename Proxy, bool Exact>
concept const_correct_call_for_proxy =
    !is_weak_data<Proxy> && voidness<CALL> && is_proxy<Proxy> &&
    ((Exact && (is_const_void<CALL> == is_const_data<Proxy>)) ||
     (!Exact && const_correct_call<is_const_void<CALL>, is_const_data<Proxy>,
                                   is_weak_data<Proxy>>));

template <is_proxy Proxy, typename From>
Proxy erased(From&& from) {
  return proxy_trait<Proxy>::erase(std::forward<From>(from));
}

template <is_proxy Proxy, typename ConstructedWith>
using unerased =
    proxy_trait<Proxy>::template unerased<std::decay_t<ConstructedWith>>;

template <is_proxy Proxy>
void const* get_proxy_ptr(Proxy const& vv, any_v_table* v_table)
  requires std::same_as<void const*, typename proxy_trait<Proxy>::void_t>
{
  return proxy_trait<Proxy>::get_proxy_ptr_in(vv, v_table);
}
template <is_proxy Proxy>
void* get_proxy_ptr(Proxy const& vv, any_v_table* v_table)
  requires std::same_as<void*, typename proxy_trait<Proxy>::void_t>
{
  return proxy_trait<Proxy>::get_proxy_ptr_in(vv, v_table);
}

template <typename U>
auto unchecked_unerase_cast(void const* p) {
  return static_cast<U const*>(p);
}
template <typename U>
auto unchecked_unerase_cast(void* p) {
  return static_cast<U*>(p);
}

template <typename U, is_proxy Proxy>
auto unchecked_unerase_cast(Proxy const& o, any_v_table* v_table) {
  return unchecked_unerase_cast<U>(get_proxy_ptr(o, v_table));
}
template <typename U, is_proxy Proxy>
auto unchecked_unerase_cast(Proxy const& o, any_v_table* v_table)
  requires(!is_const_data<Proxy>)
{
  return unchecked_unerase_cast<U>(get_proxy_ptr(o, v_table));
}

template <typename U, is_proxy Proxy>
auto unerase_cast(Proxy const& o, any_v_table* v_table) {
  check_type_match<U>(v_table);
  return unchecked_unerase_cast<U>(o, v_table);
}
template <typename U, is_proxy Proxy>
U const* unerase_cast_if(Proxy const& o, any_v_table* v_table) {
  if (type_match<U>(v_table)) return unchecked_unerase_cast<U>(o, v_table);
  return nullptr;
}
template <typename U, is_proxy Proxy>
U* unerase_cast_if(Proxy const& o, any_v_table* v_table)
  requires(!is_const_data<Proxy>)
{
  if (type_match<U>(v_table)) return unchecked_unerase_cast<U>(o, v_table);
  return nullptr;
}

// --------------------------------------------------------------------------------
// (un)erased data by_val

static_assert(std::is_const_v<std::remove_reference_t<int const&>>);
static_assert(!std::is_const_v<std::remove_reference_t<int&>>);
static_assert(!std::is_const_v<std::remove_reference_t<int>>);

template <typename V>
struct proxy_trait<by_val<V>> : basic_proxy_trait<by_val<V>> {
  using void_t = std::conditional_t<std::is_const_v<std::remove_reference_t<V>>,
                                    const_void, mutable_void>;
  using static_dispatch_t = V;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = std::is_constructible_v<
        V, ConstructedWith>;  // && !is_any<ConstructedWith>;
  };
  static constexpr bool is_owner = true;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] any_v_table* v_table) {
    return void_t{};
  }

  static auto get_proxy_ptr_in(auto& val,
                               [[maybe_unused]] any_v_table* v_table) {
    return &val;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  static auto construct_in_place(V&& v) { return std::move(v); }
  template <typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    return V{std::forward<Args>(args)...};
  }
  template <typename Vx>
  static auto erase(Vx&& v) {
    return by_val<V>{std::forward<Vx>(v)};
  }
};

// --------------------------------------------------------------------------------
// erased data variant

template <template <typename> typename Any, is_proxy Proxy, typename... Types>
using vany_variant = std::variant<Any<Proxy>, Types...>;

template <template <typename> typename Any, is_proxy Proxy, typename... Types>
using make_vany = Any<by_val<vany_variant<Any, Proxy, Types...>>>;

template <typename VanyVariant>
struct vany_variant_trait {
  using vany_variant_val = VanyVariant;
  using vany_variant = typename VanyVariant::value_t;
  template <typename... Types>
  struct concrete_variant_impl;
  template <typename First, typename... Types>
  struct concrete_variant_impl<std::variant<First, Types...>> {
    using type = std::variant<Types...>;
  };
  using concrete_variant = typename concrete_variant_impl<vany_variant>::type;
  using any_in_variant = typename std::variant_alternative_t<0, vany_variant>;
};

template <typename Vany>
struct vany_type_trait {
  using vany = Vany;
  using vany_variant = typename Vany::proxy_t;
  using concrete_variant =
      typename vany_variant_trait<vany_variant>::concrete_variant;
  using any_in_variant =
      typename vany_variant_trait<vany_variant>::any_in_variant;
};

template <template <typename> typename Any, is_proxy Proxy, typename... Types>
struct proxy_trait<by_val<vany_variant<Any, Proxy, Types...>>>
    : basic_proxy_trait<by_val<vany_variant<Any, Proxy, Types...>>> {
  using vany_variant_t = vany_variant<Any, Proxy, Types...>;
  using void_t = typename proxy_trait<Proxy>::void_t;
  using static_dispatch_t = vany_variant_t;
  static constexpr bool is_constructibile_from_const =
      proxy_trait<Proxy>::is_constructibile_from_const;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value =
        std::is_constructible_v<vany_variant_t, ConstructedWith>;
  };
  static constexpr bool is_owner = proxy_trait<Proxy>::is_owner;
  static constexpr bool is_weak =
      proxy_trait<Proxy>::is_weak;  // cppcheck-suppress
                                    // duplInheritedMember
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] any_v_table* v_table) {
    return void_t{};
  }

  static auto get_proxy_ptr_in(auto& val,
                               [[maybe_unused]] any_v_table* v_table) {
    return &val;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static vany_variant_t construct_in_place(V&& v) {
    return by_val<vany_variant_t>{std::forward<V>(v)};
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    return by_val<vany_variant_t>{T{std::forward<Args>(args)...}};
  }
  template <typename Vx>
  static auto erase(Vx&& v) {
    return by_val<vany_variant_t>{std::forward<Vx>(v)};
  }
};

// --------------------------------------------------------------------------------
// erased data observer

template <voidness Voidness>
using observer = Voidness;
using cref = observer<const_void>;
using mutref = observer<mutable_void>;

template <voidness Voidness>
struct observer_trait : basic_proxy_trait<Voidness> {
  using void_t = Voidness;
  using static_dispatch_t = void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  static constexpr bool is_constructibile_from_const = is_const;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = false;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] any_v_table* v_table) {
    return void_t{};
  }
  static void move_to(Voidness& to, [[maybe_unused]] auto, Voidness from,
                      [[maybe_unused]] auto) {
    to = from;
  }

  static Voidness get_proxy_ptr_in(const auto& ptr,
                                   [[maybe_unused]] any_v_table* v_table) {
    return ptr;
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static auto construct_in_place(V&&) {
    static_assert(false);
    return nullptr;
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    static_assert(false);
    return nullptr;
  }
  template <typename V>
  static auto erase(V& v) {
    static_assert(!std::is_const_v<std::remove_reference_t<V>>);
    return static_cast<Voidness>(&v);
  }
  template <typename V>
  static auto erase(const V& v)
    requires(is_const)
  {
    return static_cast<Voidness>(&v);
  }
};

template <>
struct proxy_trait<cref> : observer_trait<cref> {};
template <>
struct proxy_trait<mutref> : observer_trait<mutref> {};

static_assert(proxy_trait<cref>::is_const);
static_assert(!proxy_trait<mutref>::is_const);
static_assert(is_proxy<cref>);
static_assert(is_proxy<mutref>);
static_assert(is_proxy<mutref>);
static_assert(is_proxy<cref>);

// --------------------------------------------------------------------------------
// erased data unique

struct unique {
  mutable_void ptr = nullptr;
  explicit unique(mutable_void p = nullptr) : ptr(p) {}
  unique(unique const&) = delete;
  unique& operator=(unique const&) = delete;
  unique(unique&& other) { std::swap(ptr, other.ptr); }
  unique& operator=(unique&& other) {
    assert(!ptr);
    std::swap(ptr, other.ptr);
    return *this;
  };
  ~unique() = default;
  explicit operator bool() const { return static_cast<bool>(ptr); }
};

template <>
struct proxy_trait<unique> : basic_proxy_trait<unique> {
  using void_t = void*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = true;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] any_v_table* v_table) {
    return unique{copy_construct(v_table, data_ptr)};
  }
  static void move_to(unique& to, any_v_table* v_table_to, unique&& from,
                      [[maybe_unused]] any_v_table* v_table_from) {
    mutable_void old = nullptr;
    std::swap(to.ptr, old);
    std::swap(to.ptr, from.ptr);
    delete_(v_table_to, old);
  }

  static void* get_proxy_ptr_in(const auto& ptr,
                                [[maybe_unused]] any_v_table* v_table) {
    return ptr.ptr;
  }

  static void destroy(unique& u, any_v_table* v_table) {
    assert(v_table || !u.ptr);
    if (v_table) delete_(v_table, u.ptr);
  }

  template <typename ConstructedWith>
  struct unerased_impl {
    using type = std::decay_t<ConstructedWith>;
  };
  template <typename V>
  struct unerased_impl<std::unique_ptr<V>> {
    using type = std::decay_t<V>;
  };
  template <typename ConstructedWith>
  using unerased = unerased_impl<ConstructedWith>::type;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return unique{new V{std::forward<V>(v)}};
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return unique{new T{std::forward<Args>(args)...}};
  }
  template <typename V>
  static auto erase(std::unique_ptr<V>&& v) {
    return unique{v.release()};
  }
};

static_assert(is_proxy<unique>);

// --------------------------------------------------------------------------------
// erased data shared + weak

using shared = std::shared_ptr<void const>;
using weak = std::weak_ptr<void const>;

template <>
struct proxy_trait<shared> : basic_proxy_trait<shared> {
  using void_t = void const*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = true;
  static auto clone_from(const_void data_ptr, any_v_table* v_table) {
    return shared{copy_construct(v_table, data_ptr), v_table->delete_};
  }
  static void move_to(shared& to, [[maybe_unused]] any_v_table* v_table_to,
                      shared&& from, [[maybe_unused]] auto) {
    to = std::move(from);
  }
  static void move_to(shared& to, [[maybe_unused]] any_v_table* v_table_to,
                      unique from, any_v_table* v_table) {
    mutable_void p = nullptr;
    std::swap(from.ptr, p);
    to = shared{p, v_table->delete_};
  }

  static void const* get_proxy_ptr_in(const auto& v,
                                      [[maybe_unused]] any_v_table* v_table) {
    return v.get();
  }

  template <typename ConstructedWith>
  struct unerased_impl {
    using type = std::decay_t<ConstructedWith>;
  };
  template <typename V>
  struct unerased_impl<std::shared_ptr<V>> {
    using type = std::decay_t<V>;
  };
  template <typename ConstructedWith>
  using unerased = unerased_impl<ConstructedWith>::type;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return std::make_shared<V>(std::forward<V>(v));
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
  }
  template <typename V>
  static auto erase(std::shared_ptr<V> const& v) {
    return static_pointer_cast<void const>(v);
  }
};

template <>
struct proxy_trait<weak> : basic_proxy_trait<weak> {
  using void_t = void const*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = false;
  static constexpr bool is_weak =  // NOLINT([duplInheritedMember)
      true;                        // cppcheck-suppress duplInheritedMember
  static auto clone_from([[maybe_unused]] const_void data_ptr,  // NOLINT
                         [[maybe_unused]] any_v_table* v_table) {
    return weak{};
  }

  static void const* get_proxy_ptr_in([[maybe_unused]] const auto& ptr,
                                      [[maybe_unused]] any_v_table* v_table) {
    return nullptr;
  }

  template <typename ConstructedWith>
  using unerased = std::decay_t<typename ConstructedWith::element_type>;

  template <typename V>
  static auto construct_in_place(V&&) {
    static_assert(false);
    return nullptr;
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place([[maybe_unused]] Args&&... args) {
    static_assert(false);
    return nullptr;
  }
  template <typename V>
  static auto erase(std::shared_ptr<V> const& v) {
    return weak{static_pointer_cast<void const>(v)};
  }
  template <typename V>
  static auto erase(std::weak_ptr<V> const& v) {
    return erase(v.lock());
  }
};

static_assert(is_proxy<shared>);
static_assert(is_proxy<weak>);

// --------------------------------------------------------------------------------
// erased data val

struct heap_data {
  mutable_void ptr = nullptr;
  heap_data(heap_data const&) {}  // NOLINT(missingMemberCopy)
  heap_data& operator=(heap_data const&) {
    assert(!ptr);
    return *this;
  }
  explicit heap_data(mutable_void p = nullptr) : ptr(p) {}
  heap_data(heap_data&& other) { std::swap(ptr, other.ptr); }
  heap_data& operator=(heap_data&& other) {
    assert(!ptr);
    std::swap(ptr, other.ptr);
    return *this;
  };
  ~heap_data() = default;
  mutable_void release() noexcept {
    mutable_void p = ptr;
    ptr = nullptr;
    return p;
  }
  friend void swap(heap_data& l, heap_data& r) noexcept { std::swap(l, r); }
};

template <bool Trivial>
struct local_data : std::array<std::byte, sizeof(mutable_void)> {
  static constexpr inline bool is_trivial = Trivial;
};

union val {
  val(mutable_void ptr = 0) : heap{ptr} {}
  val([[maybe_unused]] val const& other) noexcept { trivial = other.trivial; }
  val& operator=([[maybe_unused]] val const& other) noexcept {
    trivial = other.trivial;
    return *this;
  }
  ~val() {}
  heap_data heap;
  local_data<false> local;
  local_data<true> trivial;
};

template <typename V>
auto visit_value(auto&& visitor, V&& v, std::size_t size) -> decltype(auto) {
  if (size > sizeof(mutable_void)) {
    return std::forward<decltype(visitor)>(visitor)(std::forward<V>(v).heap);
  } else if (size > 0) {
    return std::forward<decltype(visitor)>(visitor)(std::forward<V>(v).local);
  }
  assert(size == 0);
  return std::forward<decltype(visitor)>(visitor)(std::forward<V>(v).trivial);
};

template <typename V2>
auto visit_value(auto&& visitor, val& v1, std::size_t size1, V2&& v2,
                 std::size_t size2) -> decltype(auto) {
  if (size1 > sizeof(mutable_void)) {
    if (size2 > sizeof(mutable_void)) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.heap, std::forward<V2>(v2).heap);
    } else if (size2 > 0) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.heap, std::forward<V2>(v2).local);
    }
    assert(size2 == 0);
    return std::forward<decltype(visitor)>(visitor)(
        v1.heap, std::forward<V2>(v2).trivial);
  } else if (size1 > 0) {
    if (size2 > sizeof(mutable_void)) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.local, std::forward<V2>(v2).heap);
    } else if (size2 > 0) {
      return std::forward<decltype(visitor)>(visitor)(
          v1.local, std::forward<V2>(v2).local);
    }
    assert(size2 == 0);
    return std::forward<decltype(visitor)>(visitor)(
        v1.local, std::forward<V2>(v2).trivial);
  }
  assert(size1 == 0);
  if (size2 > sizeof(mutable_void)) {
    return std::forward<decltype(visitor)>(visitor)(v1.trivial,
                                                    std::forward<V2>(v2).heap);
  } else if (size2 > 0) {
    return std::forward<decltype(visitor)>(visitor)(v1.trivial,
                                                    std::forward<V2>(v2).local);
  }
  assert(size2 == 0);
  return std::forward<decltype(visitor)>(visitor)(v1.trivial,
                                                  std::forward<V2>(v2).trivial);
};

template <typename T, typename... Args>
auto make_local_value(Args&&... args) {
  static_assert(alignof(T) <= alignof(mutable_void));
  static_assert(sizeof(T) <= sizeof(mutable_void));
  constexpr bool is_trivial = std::is_trivial_v<T>;
  using local_data_type = local_data<is_trivial>;
  val v;
  auto location = static_cast<T*>(static_cast<mutable_void>(v.local.data()));
  std::construct_at<T>(location, std::forward<Args>(args)...);
  return v;
}

template <typename T, typename... Args>
auto make_value(Args&&... args) {
  static_assert(alignof(T) <= alignof(mutable_void));
  if constexpr (sizeof(T) <= sizeof(mutable_void)) {
    return make_local_value<T>(std::forward<Args>(args)...);
  } else {
    return val{new T(std::forward<Args>(args)...)};
  }
}

template <>
struct proxy_trait<val> : basic_proxy_trait<val> {
  using void_t = void*;
  using static_dispatch_t = void_t;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  template <typename ConstructedWith>
  struct is_constructibile_from {
    static constexpr bool value = false;
  };
  static constexpr bool is_owner = true;
  static auto clone_from([[maybe_unused]] const_void data_ptr,
                         [[maybe_unused]] any_v_table* v_table) {
    assert(v_table);
    anyxx::val v;
    visit_value(overloads{[&](heap_data& heap) {
                            heap.ptr = copy_construct(v_table, data_ptr);
                          },
                          [&]<bool Trivial>(local_data<Trivial>& local) {
                            copy_construct_at(
                                v_table,
                                static_cast<mutable_void>(local.data()),
                                data_ptr);
                          }},
                v, v_table->model_size);
    return v;
  }

  static void move_to(val& to, [[maybe_unused]] any_v_table* v_table_to,
                      val&& from, [[maybe_unused]] any_v_table* v_table_from) {
    if (!v_table_from && !v_table_to) return;
    visit_value(
        overloads{
            [&](heap_data& t, heap_data& f) {
              heap_data old;
              std::swap(t, old);
              std::swap(t, f);
              delete_(v_table_to, old.ptr);
            },
            [&](heap_data& t, local_data<false>& f) {
              delete_(v_table_to, t.ptr);
              v_table_from->move_constructor(to.local.data(), f.data());
            },
            [&](heap_data& t, local_data<true>& f) {
              delete_(v_table_to, t.ptr);
              to.trivial = std::move(f);
            },
            [&](local_data<false>& t, heap_data& f) {
              if (v_table_to) v_table_to->destructor(t.data());
              to.heap.ptr = f.release();
            },
            [&](local_data<false>& t, local_data<false>& f) {
              if (v_table_to) v_table_to->destructor(t.data());
              v_table_from->move_constructor(t.data(), f.data());
            },
            [&](local_data<false>& t, local_data<true>& f) {
              if (v_table_to) v_table_to->destructor(t.data());
              to.trivial = std::move(f);
            },
            [&]([[maybe_unused]] local_data<true>& t, heap_data& f) {
              to.heap.ptr = f.release();
            },
            [&]([[maybe_unused]] local_data<true>& t, local_data<false>& f) {
              v_table_from->move_constructor(to.local.data(), f.data());
            },
            [&](local_data<true>& t, local_data<true>& f) {
              t = std::move(f);
            }},
        to, model_size(v_table_to), from, v_table_from->model_size);
  }
  static void move_to(unique& to, any_v_table* to_v_table, val&& v,
                      any_v_table* v_table) {
    assert(v_table);
    auto data_ptr =
        visit_value(overloads{[&](heap_data& heap) { return heap.release(); },
                              [&]<bool Trivial>(local_data<Trivial>& local) {
                                return move_construct(v_table, local.data());
                              }},
                    v, v_table->model_size);
    proxy_trait<unique>::move_to(to, to_v_table, unique{data_ptr}, v_table);
  }

  static void copy_construct_from(val& to, any_v_table* to_v_table,
                                  val const& from, any_v_table* from_v_table) {
    if (!from_v_table) return;
    visit_value(
        overloads{
            [&](heap_data& to_data, heap_data const& from_data) {
              delete_(to_v_table, to_data.ptr);
              to_data.ptr = nullptr;
              if (from_data.ptr)
                to_data.ptr = copy_construct(from_v_table, from_data.ptr);
            },
            [&](heap_data& t, local_data<false> const& f) {
              delete_(to_v_table, t.ptr);
              from_v_table->copy_constructor(to.local.data(), f.data());
            },
            [&](heap_data& t, local_data<true> const& f) {
              delete_(to_v_table, t.ptr);
              to.trivial = f;
            },
            [&](local_data<false>& t, heap_data const& f) {
              if (to_v_table) to_v_table->destructor(t.data());
              to.heap.ptr = copy_construct(from_v_table, f.ptr);
            },
            [&](local_data<false>& t, local_data<false> const& f) {
              if (to_v_table) to_v_table->destructor(t.data());
              from_v_table->copy_constructor(t.data(), f.data());
            },
            [&](local_data<false>& t, local_data<true> const& f) {
              if (to_v_table) to_v_table->destructor(t.data());
              to.trivial = f;
            },
            [&]([[maybe_unused]] local_data<true>& t, heap_data const& f) {
              to.heap.ptr = copy_construct(from_v_table, f.ptr);
            },
            [&]([[maybe_unused]] local_data<true>& t,
                local_data<false> const& f) {
              from_v_table->copy_constructor(to.local.data(), f.data());
            },
            [&](local_data<true>& t, local_data<true> const& f) { t = f; }},
        to, model_size(to_v_table), from, from_v_table->model_size);
  }

  static void destroy(val& v, any_v_table* v_table) {
    visit_value(overloads{[&](heap_data& heap) {
                            assert(v_table || !heap.ptr);
                            if (v_table) delete_(v_table, heap.ptr);
                          },
                          [&](local_data<false>& local) {
                            if (v_table) v_table->destructor(local.data());
                          },
                          [&]([[maybe_unused]] local_data<true>& local) {}},
                v, model_size(v_table));
  }

  template <typename V>
  static void* get_proxy_ptr_in(V&& v, [[maybe_unused]] any_v_table* v_table) {
    if (!v_table) return nullptr;
    auto model_size = v_table->model_size;
    return visit_value(
        overloads{[&](heap_data const& heap) { return heap.ptr; },
                  [&]<bool Trivial>(
                      local_data<Trivial> const& local) -> mutable_void {
                    return static_cast<mutable_void>(
                        const_cast<std::byte*>(local.data()));
                  }},
        std::forward<V>(v), model_size);
  }

  template <typename ConstructedWith>
  using unerased = ConstructedWith;

  template <typename V>
  static auto construct_in_place(V&& v) {
    return make_value<V>(std::forward<V>(v));
  }
  template <typename T, typename... Args>
  static auto construct_type_in_place(Args&&... args) {
    return make_value<T>(std::forward<Args>(args)...);
  }
  template <typename ConstructedWith>
  static auto erase(ConstructedWith&& v) {
    return make_value<std::decay_t<ConstructedWith>>(
        std::forward<ConstructedWith>(v));
  }
};

static_assert(is_proxy<val>);

// --------------------------------------------------------------------------------
// meta data

class meta_data;

template <typename TYPE>
auto& runtime_implementation();

#ifdef ANY_DLL_MODE
template <typename T>
meta_data& get_meta_data();
#else
template <typename T>
meta_data& get_meta_data() {
  return runtime_implementation<std::decay_t<T>>();
}
#endif

template <bool dynamic, typename Trait>
struct v_table_holder;
template <typename Trait>
struct v_table_holder<false, Trait> {
  struct v_table_t {};

  v_table_holder() = default;
  explicit v_table_holder(v_table_t*) {}
  static void set_v_table_ptr(auto) {}
  static auto get_v_table_ptr() { return nullptr; }
  template <typename...>
  static void init_v_table() {}
  static auto release_v_table() { return nullptr; }
};
// template <>
// struct v_table_holder<true, base_trait> {
//   v_table_holder() = default;
//   explicit v_table_holder(any_v_table* v_table) : v_table_(v_table) {}
//   void set_v_table_ptr(any_v_table* v_table) { v_table_ = v_table; }
//   using v_table_t = any_v_table;
//   v_table_t* v_table_ = nullptr;
//   // cppcheck-suppress-begin [functionConst, functionStatic]
//   template <typename Derived>
//   auto get_v_table_ptr(this Derived const& self) {  // NOLINT
//     static_assert(std::derived_from<typename Derived::v_table_t, v_table_t>);
//     return static_cast<typename Derived::v_table_t*>(self.v_table_);
//   }
//   // cppcheck-suppress-end [functionConst, functionStatic]
//   template <typename Proxy, typename Concrete, typename Self>
//   void init_v_table(this Self& self) {
//     using derived_v_table_t = typename Self::v_table_t;
//     self.v_table_ = v_table_instance<derived_v_table_t,
//                                      anyxx::unerased<Proxy,
//                                      Concrete>>();
//   }
//   auto release_v_table() { return std::exchange(v_table_, nullptr); }
// };

template <typename VTable>
void set_is_derived_from(auto v_table) {
  v_table->is_derived_from_ = +[](const std::type_info& from) {
    return VTable::static_is_derived_from(from);
  };
}

template <typename Trait>
struct with_open_dispatch : std::false_type {};

template <typename Trait>
struct v_table_holder<true, Trait> {
 public:
  using v_table_t = Trait::v_table_t;

 private:
  v_table_t* v_table_ = nullptr;

 public:
  v_table_holder() = default;
  explicit v_table_holder(v_table_t* v_table) : v_table_(v_table) {}
  void set_v_table_ptr(v_table_t* v_table) { v_table_ = v_table; }
  // cppcheck-suppress-begin [functionConst, functionStatic]
  auto get_v_table_ptr() const { return v_table_; }
  // cppcheck-suppress-end [functionConst, functionStatic]
  template <typename Proxy, typename Concrete>
  void init_v_table() {
    v_table_ = v_table_instance<v_table_t, anyxx::unerased<Proxy, Concrete>>();
  }
  auto release_v_table() { return std::exchange(v_table_, nullptr); }
};

void insert_function(dispatch_table_t* table, std::size_t index, auto fp) {
  if (table->size() <= index) table->resize(index + 1);
  auto& entry = table->at(index);
  entry = reinterpret_cast<unsigned long long>(fp);
}
inline dispatch_table_function_t get_function(dispatch_table_t* table,
                                              std::size_t index) {
  if (table->size() <= index) return {};
  return reinterpret_cast<dispatch_table_function_t>(table->at(index));
}

inline dispatch_table_dispatch_index_t get_multi_dispatch_index_at(
    dispatch_table_t* table, std::size_t index) {
  if (table->size() <= index) return {};
  if (auto const entry = table->at(index))
    return static_cast<dispatch_table_dispatch_index_t>(entry);
  else
    return {};
}
inline void set_multi_dispatch_index_at(
    dispatch_table_t* table, std::size_t index_multi_dispatch,
    dispatch_table_dispatch_index_t
        dispatch_index_of_class_in_dispatch_matrix) {
  if (table->size() <= index_multi_dispatch)
    table->resize(index_multi_dispatch + 1);
  auto& entry = table->at(index_multi_dispatch);
  entry = dispatch_index_of_class_in_dispatch_matrix;
}

struct cast_error {
  std::type_info const &to, &from;
};

class meta_data {
  const std::type_info& type_info_;
  std::vector<any_v_table*> i_table_;

 public:
  template <typename CLASS>
  explicit constexpr meta_data(std::in_place_type_t<CLASS>)
      : type_info_(typeid(CLASS)) {}

  constexpr const std::type_info& get_type_info() const { return type_info_; }

  auto& get_i_table() { return i_table_; }
  auto& get_i_table() const { return i_table_; }

  std::expected<any_v_table*, cast_error> get_v_table(
      std::type_info const& typeid_) const {
    auto const& i_table = get_i_table();
    for (auto v_table : i_table)
      if (is_derived_from(typeid_, v_table)) return v_table;
    return std::unexpected(cast_error{.to = typeid_, .from = get_type_info()});
  }
  auto register_v_table(any_v_table* v_table) {
    v_table->meta_data_ = this;
    if (std::ranges::find(get_i_table(), v_table) == get_i_table().end())
      i_table_.push_back(v_table);
    return v_table;
  }
};

template <typename TYPE>
auto& runtime_implementation() {
  static meta_data meta_data_{std::in_place_type<TYPE>};
  return meta_data_;
}

template <typename VTable, typename Concrete>
auto bind_v_table_to_meta_data() {
  auto v_table = v_table_instance<VTable, Concrete>();
  get_meta_data<Concrete>().register_v_table(v_table);
  return v_table;
}

template <typename U>
bool type_match(any_v_table* v_table) {
  return v_table->get_type_info() == typeid(std::decay_t<U>);
}

// --------------------------------------------------------------------------------
// borrow erased data

template <is_proxy To, is_proxy From>
struct borrow_trait;

template <typename To, typename From>
concept borrowable_from =
    is_proxy<From> && is_proxy<To> && requires(From f, any_v_table* v_table) {
      { borrow_trait<To, From>{}(f, v_table) } -> std::same_as<To>;
    };

template <typename To, typename From>
  requires borrowable_from<To, From>
To borrow_as(From const& from, any_v_table* v_table) {
  return borrow_trait<To, From>{}(from, v_table);
}

template <is_proxy From>
  requires(!is_const_data<From> && !is_weak_data<From>)
struct borrow_trait<mutref, From> {
  auto operator()(const auto& from, any_v_table* v_table) const {
    return mutref{get_proxy_ptr(from, v_table)};
  }
};
template <is_proxy From>
  requires(!is_weak_data<From>)
struct borrow_trait<cref, From> {
  auto operator()(const auto& from,
                  [[maybe_unused]] any_v_table* v_table) const {
    return cref{get_proxy_ptr(from, v_table)};
  }
};
template <>
struct borrow_trait<shared, shared> {
  auto operator()(const auto& from,
                  [[maybe_unused]] any_v_table* v_table) const {
    return from;
  }
};
template <>
struct borrow_trait<weak, weak> {
  auto operator()(const auto& from,
                  [[maybe_unused]] any_v_table* v_table) const {
    return from;
  }
};
template <>
struct borrow_trait<weak, shared> {
  auto operator()(const auto& from,
                  [[maybe_unused]] any_v_table* v_table) const {
    return weak{from};
  }
};

static_assert(!borrowable_from<mutref, cref>);
static_assert(borrowable_from<mutref, mutref>);
static_assert(borrowable_from<mutref, unique>);
static_assert(!borrowable_from<mutref, shared>);
static_assert(!borrowable_from<mutref, weak>);
static_assert(borrowable_from<mutref, val>);

static_assert(borrowable_from<cref, cref>);
static_assert(borrowable_from<cref, mutref>);
static_assert(borrowable_from<cref, unique>);
static_assert(borrowable_from<cref, shared>);
static_assert(!borrowable_from<cref, weak>);
static_assert(borrowable_from<cref, val>);

static_assert(!borrowable_from<shared, cref>);
static_assert(!borrowable_from<shared, mutref>);
static_assert(!borrowable_from<shared, unique>);
static_assert(borrowable_from<shared, shared>);
static_assert(!borrowable_from<shared, weak>);
static_assert(!borrowable_from<shared, val>);

static_assert(!borrowable_from<weak, cref>);
static_assert(!borrowable_from<weak, mutref>);
static_assert(!borrowable_from<weak, unique>);
static_assert(borrowable_from<weak, shared>);
static_assert(borrowable_from<weak, weak>);
static_assert(!borrowable_from<weak, val>);

static_assert(!borrowable_from<unique, cref>);
static_assert(!borrowable_from<unique, mutref>);
static_assert(!borrowable_from<unique, unique>);
static_assert(!borrowable_from<unique, shared>);
static_assert(!borrowable_from<unique, weak>);
static_assert(!borrowable_from<unique, val>);

static_assert(!borrowable_from<val, cref>);
static_assert(!borrowable_from<val, mutref>);
static_assert(!borrowable_from<val, unique>);
static_assert(!borrowable_from<val, shared>);
static_assert(!borrowable_from<val, weak>);
static_assert(!borrowable_from<val, val>);

// --------------------------------------------------------------------------------
// clone erased data

template <is_proxy TOFROM>
struct can_copy_to;

template <typename To>
concept cloneable_to = is_proxy<To> && proxy_trait<To>::is_owner;

template <is_proxy To, is_proxy From>
  requires cloneable_to<To>
To clone_to(From const& from, any_v_table* v_table) {
  return proxy_trait<To>::clone_from(get_proxy_ptr(from, v_table), v_table);
}

static_assert(!cloneable_to<mutref>);
static_assert(!cloneable_to<cref>);
static_assert(cloneable_to<shared>);
static_assert(!cloneable_to<weak>);
static_assert(cloneable_to<unique>);
static_assert(cloneable_to<val>);

// --------------------------------------------------------------------------------
// move erased data

template <typename To, typename From>
inline static bool constexpr can_move_to_from = false;

template <typename To, typename From>
concept moveable_from =
    is_proxy<From> && is_proxy<To> && can_move_to_from<To, From>;

template <is_proxy X>
inline static bool constexpr can_move_to_from<X, X> = true;

template <>
inline bool constexpr can_move_to_from<shared, unique> = true;

template <>
inline bool constexpr can_move_to_from<weak, shared> = true;

template <voidness To, voidness From>
  requires const_correct_call<is_const_void<To>, is_const_void<From>,
                              is_weak_data<From>>
inline static bool constexpr can_move_to_from<To, From> = true;

template <is_proxy To, is_proxy From>
  requires moveable_from<To, std::decay_t<From>>
void move_to(To& to, any_v_table* to_v_table, From&& from,
             any_v_table* from_v_table) {
  return proxy_trait<From>::move_to(to, to_v_table, std::move(from),
                                    from_v_table);
}

static_assert(!moveable_from<mutref, cref>);
static_assert(moveable_from<mutref, mutref>);
static_assert(!moveable_from<mutref, unique>);
static_assert(!moveable_from<mutref, shared>);
static_assert(!moveable_from<mutref, weak>);
static_assert(!moveable_from<mutref, val>);

static_assert(moveable_from<cref, cref>);
static_assert(moveable_from<cref, mutref>);
static_assert(!moveable_from<cref, unique>);
static_assert(!moveable_from<cref, shared>);
static_assert(!moveable_from<cref, weak>);
static_assert(!moveable_from<cref, val>);

static_assert(!moveable_from<shared, cref>);
static_assert(!moveable_from<shared, mutref>);
static_assert(moveable_from<shared, unique>);
static_assert(moveable_from<shared, shared>);
static_assert(!moveable_from<shared, weak>);
static_assert(!moveable_from<shared, val>);

static_assert(!moveable_from<weak, cref>);
static_assert(!moveable_from<weak, mutref>);
static_assert(!moveable_from<weak, unique>);
static_assert(moveable_from<weak, shared>);
static_assert(moveable_from<weak, weak>);
static_assert(!moveable_from<weak, val>);

static_assert(!moveable_from<unique, cref>);
static_assert(!moveable_from<unique, mutref>);
static_assert(moveable_from<unique, unique>);
static_assert(!moveable_from<unique, shared>);
static_assert(!moveable_from<unique, weak>);
static_assert(!moveable_from<unique, val>);

static_assert(!moveable_from<val, cref>);
static_assert(!moveable_from<val, mutref>);
static_assert(!moveable_from<val, unique>);
static_assert(!moveable_from<val, shared>);
static_assert(!moveable_from<val, weak>);
static_assert(moveable_from<val, val>);

// --------------------------------------------------------------------------------
// any base

template <is_proxy Proxy, typename Trait>
class any : public v_table_holder<is_dyn<Proxy>, Trait>, public Trait {
 public:
  using proxy_t = Proxy;
  using proxy_trait_t = proxy_trait<proxy_t>;
  using void_t = typename proxy_trait_t::void_t;
  using v_table_holder_t = v_table_holder<is_dyn<Proxy>, Trait>;
  using v_table_t = typename v_table_holder_t::v_table_t;
  using T = proxy_trait_t::static_dispatch_t;
  using any_value_t = any<val, Trait>;
  static constexpr bool dyn = is_dyn<Proxy>;

 protected:
  proxy_t proxy_;

 public:
  // cppcheck-suppress-begin noExplicitConstructor
  template <typename ConstructedWith>
  explicit(false) any(ConstructedWith&& constructed_with)  // NOLINT
    requires constructibile_for<ConstructedWith, Proxy> &&
             (!std::same_as<any, std::decay_t<ConstructedWith>>)
      : proxy_(
            erased<proxy_t>(std::forward<ConstructedWith>(constructed_with))) {
    v_table_holder_t::template init_v_table<Proxy, ConstructedWith>();
  }
  // cppcheck-suppress-end noExplicitConstructor
  template <typename V>
  any(std::in_place_t, V&& v)
      : proxy_(proxy_trait<Proxy>::construct_in_place(std::forward<V>(v))) {
    v_table_holder_t::template init_v_table<Proxy, V>();
  }
  template <typename T, typename... Args>
  any(std::in_place_type_t<T>, Args&&... args)
      : proxy_(proxy_trait_t::template construct_type_in_place<T>(
            std::forward<Args>(args)...)) {
    v_table_holder_t::template init_v_table<Proxy, T>();
  }

  any() = default;
  ~any() {
    proxy_trait_t::destroy(proxy_, v_table_holder_t::get_v_table_ptr());
  }

  any(const any& other)
    requires(dyn && std::copyable<proxy_t>)
      : v_table_holder_t(other.get_v_table_ptr()) {
    proxy_trait_t::copy_construct_from(proxy_, nullptr, other.proxy_,
                                       other.get_v_table_ptr());
  }
  any(const any& other)
    requires(!dyn && std::copyable<proxy_t>)
      : v_table_holder_t(other.get_v_table_ptr()), proxy_(other.proxy_) {}
  any& operator=(any const& other)
    requires std::copyable<proxy_t>
  {
    if (this == &other) return *this;
    auto const v_table_ptr = v_table_holder_t::get_v_table_ptr();
    proxy_trait_t::copy_construct_from(proxy_, v_table_ptr, other.proxy_,
                                       other.get_v_table_ptr());
    v_table_holder_t::set_v_table_ptr(other.get_v_table_ptr());
    return *this;
  }

  template <is_any Other>
  explicit(false) any(const Other& other)  // NOLINT(noExplicitConstructor)
    requires(borrowable_from<proxy_t, typename Other::proxy_t> &&
             (!is_dyn<Proxy> ||
              std::derived_from<typename Other::v_table_t, v_table_t>))
      : v_table_holder_t(other.get_v_table_ptr()),
        proxy_(borrow_as<Proxy>(other.proxy_, other.get_v_table_ptr())) {}
  template <is_any Other>
  any& operator=(Other const& other)
    requires(borrowable_from<proxy_t, typename Other::proxy_t> &&
             (!is_dyn<Proxy> ||
              std::derived_from<typename Other::v_table_t, v_table_t>))
  {
    v_table_holder_t::set_v_table_ptr(other.get_v_table_ptr());
    proxy_ = borrow_as<Proxy>(other.proxy_, other.get_v_table_ptr());
    return *this;
  }

  template <is_proxy OtherErasedData>
    requires(moveable_from<proxy_t, OtherErasedData>)
  explicit any(OtherErasedData&& proxy, v_table_t* v_table) noexcept
      : v_table_holder_t(v_table) {
    proxy_trait_t::move_to(proxy_, nullptr, std::move(proxy), v_table);
  }
  template <is_proxy OtherErasedData>
    requires(moveable_from<proxy_t, OtherErasedData> && !dyn)
  explicit any(OtherErasedData&& proxy, v_table_t* v_table) noexcept
      : v_table_holder_t(v_table), proxy_(std::move(proxy)) {}
  template <is_any Other>
  explicit(false) any(Other&& other) noexcept  // NOLINT(noExplicitConstructor)
    requires(moveable_from<proxy_t, typename Other::proxy_t> &&
             (!is_dyn<Proxy> ||
              std::derived_from<typename Other::v_table_t, v_table_t>))
      : any(std::move(other.proxy_), other.release_v_table()) {}
  template <is_any Other>
  any& operator=(Other&& other) noexcept
    requires(moveable_from<proxy_t, typename Other::proxy_t> &&
             (!is_dyn<Proxy> ||
              std::derived_from<typename Other::v_table_t, v_table_t>))
  {
    proxy_trait_t::move_to(proxy_, v_table_holder_t::get_v_table_ptr(),
                           std::move(other.proxy_), other.get_v_table_ptr());
    v_table_holder_t::set_v_table_ptr(other.release_v_table());
    return *this;
  }
  template <typename Box>
  using type_for = any<Box, Trait>;

  template <is_any Friend>
  friend inline auto& get_proxy(Friend const& any);
  template <is_any Friend>
  friend inline auto& get_proxy(Friend& any);
  template <is_any Friend>
  friend inline decltype(auto) move_proxy(Friend&& any);
  template <is_any Friend>
  friend inline auto get_proxy_ptr(Friend const& any);

  template <is_proxy Other, typename OtherTrait>
  friend class any;

  template <typename AAny>
    requires is_any<AAny> && AAny::dyn
  friend inline auto get_v_table(AAny const& any);

  template <is_any To, is_any From>
  friend inline To unchecked_downcast_to(From from)
    requires(
        std::derived_from<typename To::v_table_t, typename From::v_table_t>);

  operator decltype(auto)() const {
    if constexpr (!voidness<typename proxy_trait_t::static_dispatch_t>) {
      return proxy_.value_;
    } else {
      return &proxy_;
    }
  }
};

template <is_any Any>
inline auto& get_proxy(Any const& any) {
  return any.proxy_;
}
template <is_any Any>
inline auto& get_proxy(Any& any) {
  return any.proxy_;
}
template <typename Any>
  requires is_any<std::decay_t<Any>> && (!std::decay_t<Any>::dyn)
inline auto& get_proxy_value(Any&& any) {
  return get_proxy(std::forward<Any>(any)).value_;
}
template <is_any Any>
inline decltype(auto) move_proxy(Any&& any) {
  return std::move(any.proxy_);
}
template <is_any Any>
inline auto get_proxy_ptr(Any const& any) {
  return get_proxy_ptr(get_proxy(any), get_v_table(any));
}

template <is_any Any>
inline const auto& get_meta_data(Any const& any) {
  return *get_v_table(any)->meta_data_;
}

template <is_any Any>
inline std::type_info const& get_type_info(Any const& any) {
  return get_v_table(any)->get_type_info();
}

template <is_any Any>
bool is_derived_from(const std::type_info& from, Any const& any) {
  return get_v_table(any)->is_derived_from_(from);
}
template <is_any From, is_any Any>
bool is_derived_from(Any const& any) {
  return is_derived_from(typeid(typename From::v_table_t), any);
}

template <typename To>
  requires std::derived_from<To, any_v_table>
auto unchecked_v_table_downcast_to(any_v_table* v_table) {
  return static_cast<To*>(v_table);
}
template <is_any To>
  requires std::derived_from<typename To::v_table_t, any_v_table>
auto unchecked_v_table_downcast_to(any_v_table* v_table) {
  return unchecked_v_table_downcast_to<typename To::v_table_t>(v_table);
}

template <typename Any>
  requires is_any<Any> && Any::dyn
inline auto get_v_table(Any const& any) {
  return unchecked_v_table_downcast_to<Any>(any.get_v_table_ptr());
}

template <is_any To, is_any From>
inline To unchecked_downcast_to(From from)
  requires(std::derived_from<typename To::v_table_t, typename From::v_table_t>)
{
  return To{std::move(from.proxy_),
            unchecked_v_table_downcast_to<To>(get_v_table(from))};
}

template <is_any To, is_any From>
inline std::optional<To> downcast_to(From from)
  requires(std::derived_from<typename To::v_table_t, typename From::v_table_t>)
{
  if (is_derived_from<To>(from))
    return {unchecked_downcast_to<To>(std::move(from))};
  return {};
}

template <typename U, is_any Any>
inline auto unchecked_unerase_cast(Any const& o) {
  return unchecked_unerase_cast<U>(get_proxy(o), get_v_table(o));
}
template <typename U, typename Any>
  requires is_any<Any> && Any::dyn
inline auto unerase_cast(Any const& o) {
  return unerase_cast<U>(get_proxy(o), get_v_table(o));
}
template <typename U, typename Any>
  requires is_any<Any> && Any::dyn
inline auto unerase_cast_if(Any const& o) {
  return unerase_cast_if<U>(get_proxy(o), get_v_table(o));
}

template <typename Value>
struct by_val {
  template <typename V>
    requires(!std::same_as<std::decay_t<V>, by_val>)
  by_val(V&& v) : value_(std::forward<V>(v)) {}
  Value value_;
  using value_t = Value;
  operator Value() const { return value_; }
  template <typename Trait>
  using as = any<by_val<Value>, Trait>;
};

template <typename Trait, typename T>
auto trait_as(T&& v) {
  return any<anyxx::by_val<std::decay_t<T>>, Trait>{std::forward<T>(v)};
}

template <typename VTable, typename Concrete>
VTable* v_table_instance() {
  static VTable v_table{std::in_place_type<Concrete>};
  return &v_table;
}

template <typename I>
concept has_open_dispatch_enabeled =
    is_any<I> && I::v_table_t::open_dispatch_enabeled;

template <typename Trait>
struct dispatch_holder<false, Trait> {
  static void set_dispatch_table([[maybe_unused]] dispatch_table_t* t) {}
};
template <typename Trait>
struct dispatch_holder<true, Trait> {
  void set_dispatch_table(dispatch_table_t* t) { dispatch_table = t; }
  dispatch_table_t* dispatch_table = nullptr;
};

template <is_any ToAny>
auto query_v_table(any_v_table* from)
    -> std::expected<typename ToAny::v_table_t*, anyxx::cast_error> {
  using v_table_t = typename ToAny::v_table_t;
  if (from->is_derived_from_(typeid(v_table_t)))
    return static_cast<v_table_t*>(from);
  return from->meta_data_->get_v_table(typeid(v_table_t))
      .transform([](auto v_table) { return static_cast<v_table_t*>(v_table); });
}

template <typename ToAny>
auto query_v_table(any_v_table* from) {
  return find_v_table<ToAny>(*from->meta_data_);
}

// --------------------------------------------------------------------------------
// any parameter translation

struct self {};

template <typename Param>
struct jacket_return;

template <typename Param>
  requires(!std::is_reference_v<Param>)
struct jacket_return<Param> {
  template <typename Sig>
  static Param forward(Sig&& sig, auto&&) {
    return std::forward<Sig>(sig);
  }
};
template <typename Param>
  requires std::is_reference_v<Param>
struct jacket_return<Param> {
  template <typename Sig>
  static decltype(auto) forward(Sig&& sig, auto&&) {
    return std::forward<Sig>(sig);
  }
};
template <>
struct jacket_return<self> {
  template <typename Sig, typename Any>
  static decltype(auto) forward(Sig&& sig, Any const&) {
    return Any{std::forward<Sig>(sig)};
  }
};
template <>
struct jacket_return<self&> {
  static auto& forward(auto, auto&& any) {
    return any;  // "return *this" semantics!
  }
};

template <typename AnyValue, typename Param>
struct translate_v_table_param {
  using type = Param;
};
template <typename AnyValue>
struct translate_v_table_param<AnyValue, self const&> {
  using type = any<cref>;
};
template <typename AnyValue>
struct translate_v_table_param<AnyValue, self&> {
  using type = any<mutref>;
};
template <typename AnyValue, typename Param>
using v_table_param = typename translate_v_table_param<AnyValue, Param>::type;

template <typename AnyValue, typename Return>
struct translate_v_table_return {
  using type = Return;
};
template <typename AnyValue>
struct translate_v_table_return<AnyValue, self> {
  using type = AnyValue;
};
template <typename AnyValue>
struct translate_v_table_return<AnyValue, self&> {
  using type = int;
};
template <typename AnyValue, typename Return>
using v_table_return =
    typename translate_v_table_return<AnyValue, Return>::type;

template <typename T>
struct handle_self_ref_return {
  static T operator()() {
    static std::remove_reference_t<T> dummy;
    return dummy;
  }
};
template <>
struct handle_self_ref_return<void> {
  static void operator()() {}
};
template <>
struct handle_self_ref_return<self&> {
  static int operator()() { return 0; }
};

template <typename Concrete, typename T>
struct v_table_to_map {
  template <typename Sig>
  static decltype(auto) forward(Sig&& sig) {
    return std::forward<Sig>(sig);
  }
};
template <typename Concrete>
struct v_table_to_map<Concrete, self&> {
  template <typename Sig>
  static Concrete& forward(Sig&& sig) {
    return *unerase_cast<Concrete>(sig);
  }
};
template <typename Concrete>
struct v_table_to_map<Concrete, self const&> {
  template <typename Sig>
  static Concrete const& forward(Sig&& sig) {
    return *unerase_cast<Concrete>(sig);
  }
};

template <typename T, typename Param>
struct translate_map_return {
  using type = Param;
};
template <typename T>
struct translate_map_return<T, self> {
  using type = T;
};
template <typename T>
struct translate_map_return<T, self&> {
  using type = T&;
};
template <typename T, typename Param>
using map_return = typename translate_map_return<T, Param>::type;

template <typename, typename T>
struct forward_trait_to_map {
  template <typename Sig>
  static decltype(auto) forward(Sig&& sig) {
    return std::forward<Sig>(sig);
  }
};
template <typename Traited>
struct forward_trait_to_map<Traited, self&> {
  template <typename Sig>
  static Traited& forward(Sig&& sig) {
    return get_proxy_value(std::forward<Sig>(sig));
  }
};
template <typename Traited>
struct forward_trait_to_map<Traited, self const&> {
  template <typename Sig>
  static Traited const& forward(Sig&& sig) {
    return get_proxy_value(std::forward<Sig>(sig));
  }
};

// --------------------------------------------------------------------------------
// any customization traits

template <is_proxy Proxy = shared>
struct default_proxy {
  using type = Proxy;
};

// --------------------------------------------------------------------------------
// typed any

template <typename V, is_any Any>
struct typed_any : public Any {
  using any_t = Any;
  using proxy_t = typename any_t::proxy_t;
  using proxy_trait_t = any_t::proxy_trait_t;
  using void_t = proxy_trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  using any_t::any_t;

  // cppcheck-suppress-begin noExplicitConstructor
  explicit(false) typed_any(V const& v) : any_t(v) {}        // NOLINT
  explicit(false) typed_any(V&& v) : any_t(std::move(v)) {}  // NOLINT
  explicit(false) typed_any(any_t i) : any_t(i) {            // NOLINT
    check_type_match<V>(get_v_table(*this));
  }
  // cppcheck-suppress-end noExplicitConstructor

  value_t const& operator*() const {
    return *unchecked_unerase_cast<value_t const>(*this);
  }
  value_t const* operator->() const {
    return unchecked_unerase_cast<value_t const>(*this);
  }
  value_t const* get() const {
    return unchecked_unerase_cast<value_t const>(*this);
  }
  value_t& operator*() const
    requires(!is_const)
  {
    return *unchecked_unerase_cast<value_t>(*this);
  }
  value_t* operator->() const
    requires(!is_const)
  {
    return unchecked_unerase_cast<value_t>(*this);
  }
  value_t* get() const
    requires(!is_const)
  {
    return unchecked_unerase_cast<value_t>(*this);
  }
  explicit operator bool() const { return static_cast<bool>(this->proxy_); }
};

template <typename V, is_any Any>
auto as(Any source) {
  return typed_any<V, Any>{std::move(source)};
}

template <typename To, typename V, is_any Any>
auto as(typed_any<V, Any> source)
  requires std::convertible_to<V*, To*>
{
  if constexpr (typed_any<V, Any>::is_const) {
    return typed_any<To const, Any>{std::move(source.proxy_)};
  } else {
    return typed_any<To, Any>{std::move(source.proxy_)};
  }
}

// --------------------------------------------------------------------------------
// any borrow, clone, lock, move

template <is_any ToAny, is_proxy FromErasedData>
  requires borrowable_from<typename ToAny::proxy_t, FromErasedData>
std::expected<ToAny, cast_error> borrow_as(FromErasedData const& from,
                                           any_v_table* from_v_table) {
  using to = typename ToAny::proxy_t;
  return query_v_table<ToAny>(from_v_table).transform([&](auto v_table) {
    return ToAny{borrow_as<to>(from, v_table), v_table};
  });
}

template <is_any ToAny, is_any FromAny>
  requires borrowable_from<typename ToAny::proxy_t, typename FromAny::proxy_t>
std::expected<ToAny, cast_error> borrow_as(FromAny const& from) {
  if constexpr (std::same_as<typename ToAny::v_table_t,
                             typename FromAny::v_table_t>) {
    return {ToAny{from}};
  } else if constexpr (std::derived_from<typename ToAny::v_table_t,
                                         typename FromAny::v_table_t>) {
    if (auto to = downcast_to<ToAny>(from)) return *to;
    return borrow_as<ToAny>(get_proxy(from), get_v_table(from));
  } else {
    return borrow_as<ToAny>(get_proxy(from), get_v_table(from));
  }
}

template <is_any ToAny, is_any FromAny>
std::expected<ToAny, cast_error> clone_to(FromAny const& from) {
  using vv_to_t = typename ToAny::proxy_t;
  static_assert(is_proxy<vv_to_t>);
  return query_v_table<ToAny>(get_v_table(from)).transform([&](auto v_table) {
    return ToAny{clone_to<vv_to_t>(get_proxy(from), v_table), v_table};
  });
}

template <is_any FromAny>
  requires std::same_as<typename FromAny::proxy_t, weak>
auto lock(FromAny const& from_interface) {
  using to_interface_t = FromAny::template type_for<shared>;
  static_assert(is_any<to_interface_t>);
  using return_t = std::optional<to_interface_t>;
  if (auto locked = get_proxy(from_interface).lock())
    return return_t{
        to_interface_t{std::move(locked), get_v_table(from_interface)}};
  return return_t{};
}

template <is_any ToAny, is_any FromAny>
ToAny move_to(FromAny&& from) {
  auto to_v_table = query_v_table<ToAny>(from.release_v_table());
  return ToAny{move_proxy(std::move(from)), *to_v_table};
}

// --------------------------------------------------------------------------------
// hook

template <typename R, typename... Args>
class hook;
template <typename R, typename... Args>
class hook<R(Args...)> {
 public:
  struct connection_info {
    int id;
    hook* owner;
  };
  class connection {
    connection_info info_;
    friend class hook;

    connection(connection const&) = delete;
    connection& operator=(connection const&) = delete;

   public:
    // cppcheck-suppress-begin noExplicitConstructor
    explicit(false) connection(connection_info info) : info_(info) {}
    // cppcheck-suppress-end noExplicitConstructor
    connection& operator=(connection_info info) {
      close();
      info_ = info;
      return *this;
    }
    connection(connection&&) = default;
    connection& operator=(connection&&) = default;
    void close() {
      if (info_.owner) info_.owner->remove(info_.id);
      info_.owner = nullptr;
    }

    ~connection() { close(); }
  };

  class super {
    int index_;
    hook const& hook_;
    friend class hook;

    super(int index, hook const& hook) : index_(index), hook_(hook) {}

   public:
    explicit operator bool() const { return index_ >= 0; }
    R operator()(Args&&... args) const {
      assert(index_ >= 0);
      return hook_.callees_[((std::size_t)index_)].second(
          super{index_ - 1, hook_}, std::forward<Args>(args)...);
    }
  };

  using callee = std::function<R(super const&, Args...)>;

  R operator()(Args&&... args) const {
    assert(!callees_.empty());
    return callees_.back().second(super{((int)callees_.size()) - 2, *this},
                                  std::forward<Args>(args)...);
  }

  connection_info insert(callee const& f) {
    callees_.emplace_back(entry{next_id_, f});
    return connection_info{next_id_++, this};
  }

 private:
  void remove(int id) {
    std::erase_if(callees_, [&](auto const id_callee_pair) {
      return id_callee_pair.first == id;
    });
  }

  int next_id_ = 0;
  using entry = std::pair<int, callee>;
  std::vector<entry> callees_;
};

// --------------------------------------------------------------------------------
// factory

class unkonwn_factory_key_error : public error {
  using error::error;
};
template <typename Tag>
struct key {
  const char* label;
  friend auto operator<=>(key, key) = default;
};
template <typename T>
struct is_key_impl : std::false_type {};
template <typename T>
struct is_key_impl<key<T>> : std::true_type {};
template <typename T>
concept is_key = is_key_impl<T>::value;

template <template <typename...> typename Any, typename Key, typename... Args>
class factory {
  using unique_constructor_t = std::function<Any<unique>(Args...)>;
  using shared_const_constructor_t = std::function<Any<shared>(Args...)>;
  std::map<Key, unique_constructor_t> unique_factory_map_;
  std::map<Key, shared_const_constructor_t> shared_factory_map_;

  template <is_proxy Proxy>
  auto register_impl(auto& map, Key key, auto const& construct) {
    map[key] = [construct](Args... args) -> Any<Proxy> {
      return Any<Proxy>{std::in_place, construct(std::forward<Args>(args)...)};
    };
  };

  template <is_proxy Proxy>
  auto construct_impl(auto const& map, Key key, Args... args) {
    if (auto found = map.find(key); found != map.end())
      return found->second(std::forward<Args>(args)...);
    if constexpr (std::same_as<Key, std::string>) {
      throw unkonwn_factory_key_error{key};
    } else if constexpr (is_key<Key>) {
      throw unkonwn_factory_key_error{key.label};
    } else {
      throw unkonwn_factory_key_error{std::to_string(key)};
    }
  };

 public:
  auto register_(Key const& key, auto const& construct) {
    register_impl<unique>(unique_factory_map_, key, construct);
    register_impl<shared>(shared_factory_map_, key, construct);
    return nullptr;
  }
  template <is_proxy Proxy>
  auto construct(auto key, Args&&... args) {
    if constexpr (std::same_as<Proxy, unique>) {
      return construct_impl<Proxy>(unique_factory_map_, key,
                                   std::forward<Args>(args)...);
    } else {
      static_assert(std::same_as<Proxy, shared>);
      return construct_impl<Proxy>(shared_factory_map_, key,
                                   std::forward<Args>(args)...);
    }
  };
};

// --------------------------------------------------------------------------------
// extension member

#ifdef ANY_DLL_MODE
template <typename InObject>
std::size_t& members_count();
#else
template <typename InObject>
std::size_t& members_count() {
  static std::size_t count = 0;
  return count;
}
#endif

template <typename InObject>
struct members {
  members() : table_(members_count<InObject>()) {}
  using any_value_t = any<val>;
  std::vector<any_value_t> table_;
  template <typename Member, typename Arg>
  void set(Member member, Arg&& arg) {
    using value_t = typename Member::value_t;
    table_[member.index] =
        any_value_t{std::in_place_type<value_t>, std::forward<Arg>(arg)};
  }
  template <typename Member>
  typename Member::value_t const* get(Member member) const {
    const auto& val = table_[member.index];
    if (!val) return {};
    return unchecked_unerase_cast<typename Member::value_t>(val);
  }
  template <typename Member>
  typename Member::value_t* get(Member member) {
    auto& val = table_[member.index];
    if (!val) return {};
    return unchecked_unerase_cast<typename Member::value_t>(val);
  }
  template <typename Member>
  typename Member::value_t& operator[](Member member) {
    if (auto val = get(member)) {
      return *val;
    }
    using value_t = typename Member::value_t;
    set(member, value_t());
    return *get(member);
  }
};

template <typename InObject, typename ValueType>
struct member {
  using object_t = InObject;
  using value_t = ValueType;
  std::size_t index = members_count<InObject>()++;
};

// --------------------------------------------------------------------------------
// dispatch

#ifdef ANY_DLL_MODE
template <typename AnyVTable>
std::size_t& dispatchs_count();
#else
template <typename AnyVTable>
std::size_t& dispatchs_count() {
  static std::size_t count = 0;
  return count;
}
#endif

template <is_any Any>
struct virtual_ {
  using type = Any;
};

template <typename Arg>
struct translate_erased_function_param {
  using type = Arg;
};
template <is_any Any>
struct translate_erased_function_param<virtual_<Any>> {
  using type = typename Any::void_t;
};

template <typename RET, typename... Args>
struct translate_erased_function {
  using type = RET (*)(typename translate_erased_function_param<Args>::type...);
};

template <std::size_t I, typename First, typename... Args>
auto arg_n(First first, Args... args) {
  if constexpr (I == 0) {
    return first;
  } else {
    return arg_n<I - 1>(std::forward<Args>(args)...);
  }
}

template <std::size_t COUNT, typename... Args>
constexpr std::size_t dispatch_dimension_count = COUNT;
template <std::size_t COUNT, is_any Any, typename... Args>
constexpr std::size_t dispatch_dimension_count<COUNT, virtual_<Any>, Args...> =
    dispatch_dimension_count<COUNT + 1, Args...>;

template <typename R, typename... Classes>
struct ensure_function_ptr_from_functor_t {
  template <typename FUNCTOR, typename... Args>
  struct striped_virtuals {
    static R function(Classes&... classes, Args... args) {
      return FUNCTOR{}(classes..., args...);
    };
  };
  template <typename FUNCTOR, is_any Any, typename... Args>
  struct striped_virtuals<FUNCTOR, virtual_<Any>, Args...>
      : striped_virtuals<FUNCTOR, Args...> {};

  template <typename... Args>
  static auto instance(
      auto functor)  // if functor is a templated operator() from a
                     // stateless function object, instantiate it now!;
  {
    using functor_t = decltype(functor);
    if constexpr (std::is_pointer_v<functor_t>) {
      return functor;
    } else {
      return striped_virtuals<functor_t, Args...>::function;
    }
  }
};

template <typename... DispatchArgs>
struct args_to_tuple {
  template <typename T, typename... ActualArgs>
  auto operator()(T&& dispatch_args, ActualArgs&&... actual_args) {
    return std::tuple_cat(
        std::forward<T>(dispatch_args),
        std::make_tuple(std::forward<ActualArgs>(actual_args)...));
  }
};
template <is_any Any, typename... DispatchArgs>
struct args_to_tuple<virtual_<Any>, DispatchArgs...> {
  template <typename T, typename ACTUAL_ARG, typename... ActualArgs>
  auto operator()(T&& dispatch_args, ACTUAL_ARG&& dispatch_arg,
                  ActualArgs&&... actual_args) {
    return args_to_tuple<DispatchArgs...>{}(
        std::tuple_cat(std::forward<T>(dispatch_args),
                       std::make_tuple(get_proxy_ptr(dispatch_arg))),
        std::forward<ActualArgs>(actual_args)...);
  }
};

template <std::size_t FirstN, typename... Ts>
auto get_tuple_head(std::tuple<Ts...> from) {
  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return std::make_tuple(std::get<I>(from)...);
  }(std::make_index_sequence<FirstN>{});
}
template <std::size_t FromN, typename... Ts>
auto get_tuple_tail(std::tuple<Ts...> from) {
  return [&]<std::size_t... I>(std::index_sequence<I...>) {
    return std::make_tuple(std::get<I>(from)...);
  }(std::make_index_sequence<std::tuple_size_v<std::tuple<Ts...>> - FromN>{});
}

template <typename R, typename... FArgs>
struct dispatch_function;
template <typename R, typename... FArgs>
struct dispatch_function<R, std::tuple<FArgs...>> {
  using type = R (*)(FArgs...);
};

class no_default_function_error : public error {
  using error::error;
};

template <typename R, typename... OuterArgs>
struct dispatch_function_types {
  template <is_any... Anys>
  struct inner {
    template <typename... Args>
    struct implemenation {
      struct type {
        using function_t = hook<R(Anys const&..., Args...)>;
        static auto function() {
          return []([[maybe_unused]] auto super,
                    [[maybe_unused]] Anys const&... anys,
                    [[maybe_unused]] Args... args) -> R {
            if constexpr (std::same_as<R, void>) {
              return;
            } else {
              if constexpr (std::is_default_constructible_v<R>) {
                return R{};
              } else {
                throw no_default_function_error("no default function");
              }
            }
          };
        }
      };
    };
    template <is_any Any, typename... Args>
    struct implemenation<virtual_<Any>, Args...> : implemenation<Args...> {};
    template <typename... Args>
    using type = typename implemenation<Args...>::type;
  };

  template <typename... Args>
  struct outer {
    template <is_any... Anys>
    using type = inner<Anys...>::template type<Args...>;
  };
  template <is_any Any, typename... Args>
  struct outer<virtual_<Any>, Args...> {
    template <is_any... Anys>
    using type = outer<Args...>::template type<Any, Anys...>;
  };

  using default_type = outer<OuterArgs...>::template type<>;
};

template <typename F, typename... Args>
struct dispatch_matrix {
  using type = F;
};
template <typename DispatchMatrix, is_any Any, typename... Args>
struct dispatch_matrix<DispatchMatrix, virtual_<Any>, Args...> {
  using type =
      typename dispatch_matrix<std::vector<DispatchMatrix>, Args...>::type;
};

template <typename R, typename... Args>
struct dispatch;
template <typename R, typename... Args>
struct dispatch<R(Args...)> {
  using erased_function_t =
      typename translate_erased_function<R, Args...>::type;

  static constexpr std::size_t dimension_count =
      dispatch_dimension_count<0, Args...>;

  using dispatch_matrix_t = dispatch_matrix<erased_function_t, Args...>::type;
  dispatch_matrix_t dispatch_matrix_;

  using default_ = typename dispatch_function_types<R, Args...>::default_type;
  default_::function_t dispatch_default_hook_;
  default_::function_t::connection default_connection_ =
      dispatch_default_hook_.insert(default_::function());

  enum class kind { single, multiple };
  template <kind Kind, std::size_t Dimension, typename... DispatchArgs>
  struct dispatch_access;

  template <typename... DispatchArgs>
  struct dispatch_access<kind::multiple, dimension_count, DispatchArgs...> {
    auto define(auto fp, auto& matrix) {
      matrix = reinterpret_cast<erased_function_t>(fp);
      return fp;
    }
    template <typename F, typename ArgsTuple>
    std::optional<R> invoke(F const& target, ArgsTuple&& dispatch_args_tuple,
                            auto&&...) const {
      if (!target) return {};
      auto typed_target = reinterpret_cast<
          typename dispatch_function<R, std::decay_t<ArgsTuple>>::type>(target);
      return std::apply(typed_target,
                        std::forward<ArgsTuple>(dispatch_args_tuple));
    }
  };

  template <std::size_t Dimension, is_any Any, typename... DispatchArgs>
  struct dispatch_access<kind::multiple, Dimension, virtual_<Any>,
                         DispatchArgs...>
      : dispatch_access<kind::multiple, Dimension + 1, DispatchArgs...> {
    using interface_t = Any;
    using v_table_t = typename interface_t::v_table_t;
    using next_t =
        dispatch_access<kind::multiple, Dimension + 1, DispatchArgs...>;

    // index 0 is for the 'wildcard' functions
    std::size_t index_ = 1 + dispatchs_count<v_table_t>()++;
    std::size_t dispatch_dimension_size_ = 1;

    template <typename Class>
    std::size_t get_dispatch_index() {
      if constexpr (std::same_as<Any, Class>) {
        return 0;
      } else {
        auto dispatch_table = dispatch_table_instance<v_table_t, Class>();
        if (auto index = get_multi_dispatch_index_at(dispatch_table, index_))
          return index;
        else
          set_multi_dispatch_index_at(dispatch_table, index_,
                                      dispatch_dimension_size_);
        return dispatch_dimension_size_++;
      }
    }

    template <typename Class, typename... Classes>
    auto define(auto fp, auto& matrix) {
      auto dispatch_index = get_dispatch_index<Class>();
      if (matrix.size() <= dispatch_index) matrix.resize(dispatch_index + 1);
      return next_t::template define<Classes...>(fp, matrix[dispatch_index]);
    }

    template <typename DispatchMatrix, typename ArgsTuple,
              typename... ActualArgs>
    std::optional<R> invoke(DispatchMatrix const& target,
                            ArgsTuple&& dispatch_args_tuple, Any const& any,
                            ActualArgs&&... actual_args) const {
      auto dispatch_table = get_v_table(any)->dispatch_table;
      auto dispatch_dim = get_multi_dispatch_index_at(dispatch_table, index_);
      if (dispatch_dim && target.size() > dispatch_dim)
        if (auto found =
                next_t::invoke(target[dispatch_dim],
                               std::forward<ArgsTuple>(dispatch_args_tuple),
                               std::forward<ActualArgs>(actual_args)...))
          return found;

      if (target.size())
        return next_t::invoke(
            target[0],
            std::tuple_cat(get_tuple_head<Dimension>(dispatch_args_tuple),
                           std::make_tuple(&any),
                           get_tuple_tail<Dimension + 1>(dispatch_args_tuple)),
            std::forward<ActualArgs>(actual_args)...);
      return {};
    }
  };

  template <is_any Any, typename... AccessArgs>
  struct dispatch_access<kind::single, 0, virtual_<Any>, AccessArgs...> {
    using interface_t = Any;
    using v_table_t = typename interface_t::v_table_t;
    std::size_t index_ = dispatchs_count<v_table_t>()++;

    template <typename CLASS>
    auto define(auto fp, auto&) {
      auto v_table = dispatch_table_instance<v_table_t, CLASS>();
      insert_function(v_table, index_, fp);
      return fp;
    }

    template <typename... Other>
    R invoke(default_::function_t const& default_, Any const& any,
             Other&&... other) const {
      auto v_table = get_v_table(any)->dispatch_table;
      auto target = get_function(v_table, index_);
      if (!target)
        return std::invoke(default_, any, std::forward<Other>(other)...);
      auto erased_function = reinterpret_cast<erased_function_t>(target);
      return std::invoke(erased_function, get_proxy_ptr(any),
                         std::forward<Other>(other)...);
    }
  };

  static const constexpr kind dispatch_kind =
      (dimension_count > 1) ? kind::multiple : kind::single;
  dispatch_access<dispatch_kind, 0, Args...> dispatch_access_;

 public:
  template <typename... Classes>
  auto define(auto f) {
    auto fp = ensure_function_ptr_from_functor_t<
        R, Classes...>::template instance<Args...>(f);
    return dispatch_access_.template define<Classes...>(fp, dispatch_matrix_);
  };
  template <typename... ActualArgs>
  auto operator()(ActualArgs&&... actual_args) const {
    if constexpr (dispatch_kind == kind::multiple) {
      auto dispatch_args_tuple = args_to_tuple<Args...>{}(
          std::tuple<>{}, std::forward<ActualArgs>(actual_args)...);
      return *dispatch_access_
                  .invoke(dispatch_matrix_, dispatch_args_tuple,
                          std::forward<ActualArgs>(actual_args)...)
                  .or_else([&]() -> std::optional<R> {
                    return std::invoke(
                        dispatch_default_hook_,
                        std::forward<ActualArgs>(actual_args)...);
                  });
    } else {
      return dispatch_access_.invoke(dispatch_default_hook_,
                                     std::forward<ActualArgs>(actual_args)...);
    }
  }
  auto& get_dispatch_default_hook() { return dispatch_default_hook_; };
};

template <size_t At, typename Tuple, size_t... Is>
auto make_tuple_from_elements(Tuple&& tuple, std::index_sequence<Is...>) {
  return std::forward_as_tuple(
      std::get<Is + At>(std::forward<Tuple>(tuple))...);
}
template <size_t At, size_t N, typename Tuple>
auto make_tuple_from_elements_at(Tuple&& tuple) {
  return make_tuple_from_elements<At>(std::forward<Tuple>(tuple),
                                      std::make_index_sequence<N>{});
}

template <typename Vany, typename DynamicDispatch, auto StaticDispatch>
class dispatch_vany {
  DynamicDispatch dynamic_dispatch_;
  constexpr static const std::size_t dimension_count =
      DynamicDispatch::dimension_count;

  template <typename Vany1, typename... Args>
  auto invoke1(Vany1&& vany, Args&&... args) const {
    return std::visit(
        [&]<typename TypedArg>([[maybe_unused]] TypedArg&& arg) {
          return overloads{
              StaticDispatch,
              [&]<is_any Any, typename... Vargs>(Any&& any, Vargs&&... vargs) {
                return dynamic_dispatch_(std::forward<Any>(any),
                                         std::forward<Vargs>(vargs)...);
              }}(std::forward<TypedArg>(arg), std::forward<Args>(args)...);
        },
        get_proxy_value(std::forward<Vany1>(vany)));
  }

  template <is_any Vany1, is_any Vany2, typename... Args>
  auto invoke2(Vany1&& vany1, Vany2&& vany2, Args&&... args) const {
    using vany1_t = std::decay_t<Vany1>;
    using vany2_t = std::decay_t<Vany2>;
    using cv1_t = anyxx::vany_type_trait<vany1_t>::concrete_variant;
    using any_v1 = anyxx::vany_type_trait<vany1_t>::any_in_variant;
    using cv2_t = anyxx::vany_type_trait<vany2_t>::concrete_variant;
    using any_v2 = anyxx::vany_type_trait<vany2_t>::any_in_variant;

    auto dispatch_combined = [&]<typename DA1, typename DA2>(DA1&& da1,
                                                             DA2&& da2) {
      auto dyn_case1 = [&]<is_any A1, is_any A2, typename... VAs>(
                           A1&& a1, A2&& a2, VAs&&... vas) {
        return dynamic_dispatch_(std::forward<A1>(a1), std::forward<A2>(a2),
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case2 = [&]<is_any A1, typename A2, typename... VAs>(
                           A1&& a1, A2 a2, VAs&&... vas)
        requires std::constructible_from<cv2_t, A2>
      {
        return dynamic_dispatch_(std::forward<A1>(a1),
                                 any_v2{std::in_place, cv2_t{std::move(a2)}},
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case3 = [&]<typename A1, is_any A2, typename... VAs>(
                           A1 a1, A2&& a2, VAs&&... vas)
        requires std::constructible_from<cv1_t, A1>
      {
        return dynamic_dispatch_(any_v1{std::in_place, cv1_t{std::move(a1)}},
                                 std::forward<A2>(a2),
                                 std::forward<VAs>(vas)...);
      };
      auto dyn_case4 = [&]<typename A1, typename A2, typename... VAs>(
                           A1 a1, A2 a2, VAs&&... vas)
        requires(std::constructible_from<cv1_t, A1> &&
                 std::constructible_from<cv2_t, A2>)
      {
        return dynamic_dispatch_(any_v1{std::in_place, cv1_t{std::move(a1)}},
                                 any_v2{std::in_place, cv2_t{std::move(a2)}},
                                 std::forward<VAs>(vas)...);
      };
      return overloads{StaticDispatch, dyn_case1, dyn_case2, dyn_case3,
                       dyn_case4}(std::forward<DA1>(da1),
                                  std::forward<DA2>(da2),
                                  std::forward<Args>(args)...);
    };

    return std::visit(dispatch_combined,
                      get_proxy_value(std::forward<Vany1>(vany1)),
                      get_proxy_value(std::forward<Vany2>(vany2)));
  }

 public:
  template <typename... Classes>
  auto define(auto f) {
    return dynamic_dispatch_.template define<Classes...>(f);
  }

  template <typename... Args>
  auto operator()(Args&&... args) const {
    if constexpr (dimension_count == 1) {
      return invoke1(std::forward<Args>(args)...);
    } else {
      if constexpr (dimension_count == 2) {
        return invoke2(std::forward<Args>(args)...);
      } else {
        static_assert(dimension_count <= 2,
                      "dispatch_vany only supports one and two dimensions");
      }
    }
  }
};
}  // namespace anyxx

#define ANY_MERGE_(a, b) a##b
#define ANY_LABEL_(a) ANY_MERGE_(unique_name_, a)
#define ANY_UNIQUE_NAME_ ANY_LABEL_(__COUNTER__)
#define ANY_UNIQUE_NAME ANY_UNIQUE_NAME_
#define __ ANY_UNIQUE_NAME_

#define ANY_SINGLETON_DECLARE(export_, name, ...) \
  using name##_t = __VA_ARGS__;                   \
  export_ extern name##_t& get_##name();          \
  static inline name##_t& name = get_##name();

#define ANY_SINGLETON(namespace_, name, ...)       \
  namespace_::name##_t& namespace_::get_##name() { \
    static name##_t dispatch{__VA_ARGS__};         \
    return dispatch;                               \
  };

#define VANY_DISPACH_DECLARE(export_, name, vany, signature, static_dispatch) \
  constexpr static inline auto name##_static_dispatch =                       \
      anyxx::overloads{_detail_REMOVE_PARENS(static_dispatch)};               \
                                                                              \
  using name##_vany = vany;                                                   \
  using name##_dynamic_dispatch =                                             \
      anyxx::dispatch<_detail_REMOVE_PARENS(signature)>;                      \
                                                                              \
  ANY_SINGLETON_DECLARE(                                                      \
      , name,                                                                 \
      anyxx::dispatch_vany<name##_vany, name##_dynamic_dispatch,              \
                           name##_static_dispatch>)

#define VANY_DISPACH(namespace_, name) ANY_SINGLETON(namespace_, name);

#ifdef ANY_DLL_MODE

#define ANY_META_CLASS_FWD(export_, ...) \
  template <>                            \
  export_ anyxx::meta_data& anyxx::get_meta_data<__VA_ARGS__>();

#define ANY_META_CLASS(...)                                             \
  template <>                                                           \
  anyxx::meta_data& anyxx::get_meta_data<std::decay_t<__VA_ARGS__>>() { \
    return runtime_implementation<__VA_ARGS__>();                       \
  }

#else

#define ANY_META_CLASS_FWD(...)
#define ANY_META_CLASS(...)

#endif

#define ANY_META_CLASS_STATIC(...)  \
  ANY_META_CLASS_FWD(, __VA_ARGS__) \
  ANY_META_CLASS(__VA_ARGS__)

#ifdef ANY_DLL_MODE

#define ANY_MEMBERS_COUNT_FWD(export_, ns_, c_)  \
  namespace ns_ {                                \
  struct c_;                                     \
  }                                              \
  namespace anyxx {                              \
  template <>                                    \
  export_ std::size_t& members_count<ns_::c_>(); \
  }

#define ANY_MEMBERS_COUNT_IMPL(ns_, c_)          \
  template <>                                    \
  std::size_t& anyxx::members_count<ns_::c_>() { \
    static std::size_t count = 0;                \
    return count;                                \
  }

#define ANY_MEMBER_FWD(export_, object_, member_, type_)           \
  export_ anyxx::member<object_, type_>& _inintialize_##member_(); \
  inline const anyxx::member<object_, type_>& member_ =            \
      _inintialize_##member_();

#define ANY_MEMBER_IMPL(ns_, object_, member_, type_)            \
  anyxx::member<object_, type_>& ns_::_inintialize_##member_() { \
    static anyxx::member<object_, type_> instance;               \
    return instance;                                             \
  }

#else

#define ANY_MEMBERS_COUNT_FWD(...)
#define ANY_MEMBERS_COUNT_IMPL(...)
#define ANY_MEMBER_FWD(...)
#define ANY_MEMBER_IMPL(...)

#endif

#ifdef ANY_DLL_MODE

#define ANY_DISPATCH_COUNT_FWD(export_, ns_, any_)             \
  namespace ns_ {}                                             \
  namespace anyxx {                                            \
  template <>                                                  \
  export_ std::size_t& dispatchs_count<ns_::any_##_v_table>(); \
  }

#define ANY_DISPATCH_COUNT(ns_, any_)                          \
  template <>                                                  \
  std::size_t& anyxx::dispatchs_count<ns_::any_##_v_table>() { \
    static std::size_t count = 0;                              \
    return count;                                              \
  }

#define ANY_DISPATCH_FOR_FWD(export_, class_, interface_namespace_, \
                             interface_)                            \
  namespace anyxx {                                                 \
  template <>                                                       \
  export_ dispatch_table_t* dispatch_table_instance<                \
      interface_namespace_::interface_##_v_table, class_>();        \
  }

#define ANY_DISPATCH_FOR(class_, interface_namespace_, interface_) \
  template <>                                                      \
  anyxx::dispatch_table_t* anyxx::dispatch_table_instance<         \
      interface_namespace_::interface_##_v_table, class_>() {      \
    return dispatch_table_instance_implementation<                 \
        interface_namespace_::interface_##_v_table, class_>();     \
  }

#else

#define ANY_DISPATCH_COUNT_FWD(...)
#define ANY_DISPATCH_COUNT(...)
#define ANY_DISPATCH_FOR_FWD(...)
#define ANY_DISPATCH_FOR(...)

#endif

#define ANY_REGISTER_MODEL(class_, interface_, ...)                           \
  namespace {                                                                 \
  static auto __ = anyxx::bind_v_table_to_meta_data<                          \
      interface_##_v_table _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(__VA_ARGS__), \
      ANYXX_UNPAREN(class_)>();                                               \
  }
