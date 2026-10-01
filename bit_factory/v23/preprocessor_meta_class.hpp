#pragma once

#include <bit_factory/infra/config.hpp>

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

#define _detail_ANYXX_CONCEPT_ARG_LIST_H(b, c, f, ...)              \
  c __VA_OPT__(, _detail_ANYXX_FORWARD_PARAM_LIST_A _detail_PARENS( \
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

#define _detail_ANYXX_MAP_PARAM_LIST_H(b, c, param_type, ...)                  \
  [[maybe_unused]] auto&& c __VA_OPT__(                                        \
      , _detail_ANYXX_MAP_PARAM_LIST_A _detail_PARENS(b, _detail_CONCAT(b, c), \
                                                      __VA_ARGS__))
#define _detail_ANYXX_MAP_PARAM_LIST_A() _detail_ANYXX_MAP_PARAM_LIST_H
#define _detail_ANYXX_MAP_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_ANYXX_MAP_PARAM_LIST_H(__VA_ARGS__))

#define _detail_ANYXX_CONCEPT_PARAM_LIST_H(b, c, param_type, ...)     \
  [[maybe_unused]] anyxx::concept_arg<T, ANYXX_UNPAREN(param_type)> c \
  __VA_OPT__(, _detail_ANYXX_CONCEPT_PARAM_LIST_A _detail_PARENS(     \
                   b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_ANYXX_CONCEPT_PARAM_LIST_A() _detail_ANYXX_CONCEPT_PARAM_LIST_H
#define _detail_ANYXX_CONCEPT_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_ANYXX_CONCEPT_PARAM_LIST_H(__VA_ARGS__))

#define _detail_ANYXX_EXACT_PARAM_LIST_H(b, c, param_type, ...) \
  [[maybe_unused]] ANYXX_UNPAREN(param_type) c __VA_OPT__(      \
      , _detail_ANYXX_EXACT_PARAM_LIST_A _detail_PARENS(        \
            b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_ANYXX_EXACT_PARAM_LIST_A() _detail_ANYXX_EXACT_PARAM_LIST_H
#define _detail_ANYXX_EXACT_PARAM_LIST(...) \
  _detail_EXPAND_(_detail_ANYXX_EXACT_PARAM_LIST_H(__VA_ARGS__))

#define _detail_ANYXX_TYPENAME_PARAM_H(t) _detail_ANYXX_TYPENAME_PARAM t
#define _detail_ANYXX_TYPENAME_PARAM(t) , typename t
#define _detail_ANYXX_TYPENAME_PARAM_LIST(head, ...) \
  typename _detail_REMOVE_PARENS(head) __VA_OPT__(   \
      _detail_foreach_macro(_detail_ANYXX_TYPENAME_PARAM_H, __VA_ARGS__))

#define _detail_ANYXX_DUMMY_INT_PARAM_LIST_H(b, c, param_type, ...)     \
  int __VA_OPT__(, _detail_ANYXX_DUMMY_INT_PARAM_LIST_A _detail_PARENS( \
                       b, _detail_CONCAT(b, c), __VA_ARGS__))
#define _detail_ANYXX_DUMMY_INT_PARAM_LIST_A() \
  _detail_ANYXX_DUMMY_INT_PARAM_LIST_H
#define _detail_ANYXX_DUMMY_INT_PARAM_LIST(...) \
  __VA_OPT__(                                   \
      <_detail_ANYXX_DUMMY_INT_PARAM_LIST_H(dummy1, dummy2, __VA_ARGS__)>)

#define _detail_ANYXX_OPTIONAL_TEMPLATE(...) __VA_OPT__(template)

#define _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(...) \
  __VA_OPT__(template <_detail_ANYXX_TYPENAME_PARAM_LIST(__VA_ARGS__)>)
#define _detail_ANYXX_OPTIONAL_MORE_TYPENAMES_PARAM_LIST(...) \
  __VA_OPT__(, _detail_ANYXX_TYPENAME_PARAM_LIST(__VA_ARGS__))

#define _detail_ANYXX_TEMPLATE_ARG_H(t) _detail_ANYXX_TEMPLATE_ARG t
#define _detail_ANYXX_TEMPLATE_ARG(t) , t
#define _detail_ANYXX_TEMPLATE_ARGS1(head, ...) \
  _detail_REMOVE_PARENS(head) __VA_OPT__(       \
      _detail_foreach_macro(_detail_ANYXX_TEMPLATE_ARG_H, __VA_ARGS__))
#define _detail_ANYXX_TEMPLATE_ARGS(...) \
  __VA_OPT__(_detail_ANYXX_TEMPLATE_ARGS1(__VA_ARGS__))
#define _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(...) \
  __VA_OPT__(<_detail_ANYXX_TEMPLATE_ARGS(__VA_ARGS__)>)

#define _detail_LEAD_COMMA_H(...) __VA_OPT__(, )
#define _detail_ANYXX_FPD_H(l) _detail_ANYXX_FUNCTION_PTR_DECL l
#define _detail_ANYXX_MEMEBER_LIMP_H(l) _detail_ANYXX_LAMBDA_TO_MEMEBER_IMPL l
#define _detail_ANYXX_V_TABLE_DATA_DECL_H(l) _detail_ANYXX_V_TABLE_DATA_DECL l
#define _detail_ANYXX_V_TABLE_DATA_INIT_H(l) _detail_ANYXX_V_TABLE_DATA_INIT l

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
#define _detail_ANYXX_MAP_IMPL(access, overload, type, name, name_ext,        \
                               exact_const, const_, trait_body, mapf_concept, \
                               mapf_concept_lhs, ...)                         \
  access:                                                                     \
  template <typename Map>                                                     \
  AYXFORCEDINLINE auto name(                                                  \
      [[maybe_unused]] this Map const& map,                                   \
      [[maybe_unused]] auto const_& x __VA_OPT__(                             \
          , _detail_ANYXX_MAP_PARAM_LIST_H(a, _sig, __VA_ARGS__)))            \
      -> anyxx::map_return<T, ANYXX_UNPAREN(type)>                            \
    requires(anyxx::mapf_concept(requires(T const_ x __VA_OPT__(              \
        , _detail_ANYXX_CONCEPT_PARAM_LIST_H(a, sig_, __VA_ARGS__))) {        \
      {                                                                       \
        mapf_concept_lhs(__VA_OPT__(                                          \
            _detail_ANYXX_CONCEPT_ARG_LIST_H(a, sig_, __VA_ARGS__)))          \
      } -> std::convertible_to<anyxx::map_return<T, ANYXX_UNPAREN(type)>>;    \
    }))                                                                       \
  {                                                                           \
    using namespace anyxx;                                                    \
    return _detail_REMOVE_PARENS(trait_body)(                                 \
        __VA_OPT__(_detail_ANYXX_FORWARD_PARAM_LIST(a, _sig, __VA_ARGS__)));  \
  };

#define _detail_ANYXX_CONCEPT_FN_H(l) _detail_ANYXX_CONCEPT_FN l
#define _detail_ANYXX_CONCEPT_FN(access, overload, type, name, name_ext,      \
                                 exact_const, const_, trait_body,             \
                                 mapf_concept, mapf_concept_lhs, ...)         \
  requires requires(                                                          \
      __VA_OPT__(_detail_ANYXX_CONCEPT_PARAM_LIST_H(a, sig_, __VA_ARGS__))) { \
    {                                                                         \
      model_map.name(model __VA_OPT__(                                        \
          , _detail_ANYXX_CONCEPT_ARG_LIST_H(a, sig_, __VA_ARGS__)))          \
    } -> std::convertible_to<anyxx::map_return<T, ANYXX_UNPAREN(type)>>;      \
  };

#define _detail_ANYXX_CONCEPT_STATIC_FN_H(l) _detail_ANYXX_CONCEPT_STATIC_FN l
#define _detail_ANYXX_CONCEPT_STATIC_FN(template_params, return_type, name,   \
                                        body, ...)                            \
  requires requires(                                                          \
      __VA_OPT__(_detail_ANYXX_CONCEPT_PARAM_LIST_H(a, sig_, __VA_ARGS__))) { \
    {                                                                         \
      model_map.name(trait_class __VA_OPT__(                                  \
          , _detail_ANYXX_CONCEPT_ARG_LIST_H(a, sig_, __VA_ARGS__)))          \
    }                                                                         \
    -> std::convertible_to<anyxx::map_return<T, ANYXX_UNPAREN(return_type)>>; \
  };

#define _detail_ANYXX_CONCEPT_TYPE_H(l) _detail_ANYXX_CONCEPT_TYPE l
#define _detail_ANYXX_CONCEPT_TYPE(template_params, name, erased, default_) \
  requires !std::same_as<typename decltype(model_map)::deduced_type::       \
                             _detail_ANYXX_OPTIONAL_TEMPLATE(               \
                                 _detail_REMOVE_PARENS(template_params))    \
                                 name _detail_ANYXX_DUMMY_INT_PARAM_LIST(   \
                                     ANYXX_UNPAREN(template_params)),       \
                         anyxx::undefined>;

//_detail_ANYXX_CONCEPT_TYPE_H((), value_type, anyxx::undefined,
//(anyxx::undefined))

#define _detail_ANYXX_MAP_STATIC_H(l) _detail_ANYXX_MAP_STATIC l
#define _detail_ANYXX_MAP_STATIC(template_params, return_type, name, body, \
                                 ...)                                      \
 public:                                                                   \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(                              \
      _detail_REMOVE_PARENS(template_params)) AYXFORCEDINLINE auto         \
  name([[maybe_unused]] auto trait_class __VA_OPT__(                       \
      , _detail_ANYXX_MAP_PARAM_LIST_H(a, _sig, __VA_ARGS__)))             \
      -> anyxx::map_return<T, ANYXX_UNPAREN(return_type)> {                \
    using namespace anyxx;                                                 \
    return _detail_REMOVE_PARENS(body).template                            \
    operator()<anyxx::use_as<T, typename decltype(trait_class)::trait_t>>( \
        trait_class __VA_OPT__(                                            \
            , _detail_ANYXX_FORWARD_PARAM_LIST(a, _sig, __VA_ARGS__)));    \
  };

//_detail_ANYXX_MAP_STATIC(((A), (B)), decltype(auto), forward,
//                         ([](A&& a, B&& b) { return std::forward<A>(a); }),
//                         A&&, B&&)
// expands to ->
// template <typename A, typename B>
//    static __forceinline decltype(auto)
//        forward([[maybe_unused]] A&& _sig, [[maybe_unused]] B&& a_sig) {
//  return [](A&& a, B&& b) {
//    return std::forward<A>(a);
//  }(std::forward<decltype(_sig)>(_sig), std::forward<decltype(a_sig)>(a_sig));
//};

#define _detail_ANYXX_JACKET_STATIC_H(l) _detail_ANYXX_JACKET_STATIC l
#define _detail_ANYXX_JACKET_STATIC(template_params, return_type, name, body,  \
                                    ...)                                       \
  template <typename Self _detail_ANYXX_OPTIONAL_MORE_TYPENAMES_PARAM_LIST(    \
      _detail_REMOVE_PARENS(template_params))>                                 \
  AYXFORCEDINLINE decltype(auto) name(                                         \
      [[maybe_unused]] this Self&& self __VA_OPT__(, )                         \
          __VA_OPT__(_detail_ANYXX_JACKET_PARAM_LIST(a, _sig, __VA_ARGS__))) { \
    using self_t = std::decay_t<Self>;                                         \
    static_assert(!self_t::is_dyn);                                            \
    using T = typename self_t::T;                                              \
    using proxy_t = typename self_t::proxy_t;                                  \
    using map_t = typename self_t::template static_dispatch_map_t<T>;          \
    using traited_t = typename self_t::rep_type;                               \
    using trait_t = typename self_t::trait_t;                                  \
    return ANYXX_JACKET_RETURN(return_type)::forward(                          \
        map_t _detail_ANYXX_OPTIONAL_TEMPLATE(                                 \
            _detail_REMOVE_PARENS(template_params)){}                          \
            .name _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(                        \
                _detail_REMOVE_PARENS(template_params))(                       \
                anyxx::trait_class_<T, trait_t> __VA_OPT__(                    \
                    , _detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP(          \
                          a, _sig, __VA_ARGS__))),                             \
        std::forward<Self>(self));                                             \
  };

//_detail_ANYXX_JACKET_STATIC(((A), (B)), decltype(auto), forward,
//                            ([](A&& a, B&& b) { return std::forward<A>(a); }),
//                            A&&, B&&)
// expands to ->
//
//    template <typename Self, typename A, typename B>
//    __forceinline decltype(auto)
//        forward([[maybe_unused]] this Self const& self,
//                [[maybe_unused]] A&& _sig, [[maybe_unused]] B&& a_sig) {
//    static_assert(!Self::is_dyn);
//    using map_t = typename Self::static_dispatch_map_t;
//    return map_t::template forward<A, B>(std::forward<decltype(_sig)>(_sig),
//                                       std::forward<decltype(a_sig)>(a_sig));
//  };

#define _detail_ANYXX_MAP_TYPE_H(l) _detail_ANYXX_MAP_TYPE l
#define _detail_ANYXX_MAP_TYPE(template_params, name, erased, default_) \
 public:                                                                \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(_detail_REMOVE_PARENS(     \
      template_params)) using name = _detail_REMOVE_PARENS(default_);

#define _detail_ANYXX_V_TABLE_TYPE_H(l) _detail_ANYXX_V_TABLE_TYPE l
#define _detail_ANYXX_V_TABLE_TYPE(template_params, name, erased, default_) \
 public:                                                                    \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(                               \
      _detail_REMOVE_PARENS(template_params)) using name = erased;

//_detail_ANYXX_MAP_TYPE(((A), (B)), xyz, void, (std::map<A, B>))
// ->
// template <typename A, typename B>
// using xyz = std::map<A, B>;

#define _detail_ANYXX_JACKET_TYPE_H(l) _detail_ANYXX_JACKET_TYPE l
#define _detail_ANYXX_JACKET_TYPE(template_params, name, erased, default_) \
  template <typename Q _detail_ANYXX_OPTIONAL_MORE_TYPENAMES_PARAM_LIST(   \
      _detail_REMOVE_PARENS(template_params))>                             \
  using name = std::conditional_t<                                         \
      std::same_as<void, std::remove_const_t<std::remove_pointer_t<Q>>>,   \
      erased,                                                              \
      typename static_dispatch_map_t<Q>::deduced_type::                    \
          _detail_ANYXX_OPTIONAL_TEMPLATE(                                 \
              _detail_REMOVE_PARENS(template_params))                      \
              name _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(                   \
                  _detail_REMOVE_PARENS(template_params))>;

//_detail_ANYXX_JACKET_TYPE(((A),(B)), xyz, void, (std::map<A,B>))
// ->
// template <typename Self, typename A, typename B>
// using xyz = std::conditional_t<
//    Self::is_dyn, void, typename Self::static_dispatch_map_t::template xyz<A,
//    B>>;

#define _detail_ANYXX_MAP_VARIANT_LIMP_H(l) _detail_ANYXX_MAP_VARIANT_IMPL l
#define _detail_ANYXX_MAP_VARIANT_IMPL(access, overload, type, name, name_ext, \
                                       exact_const, const_, trait_body,        \
                                       mapf_concept, mapf_concept_lhs, ...)    \
  AYXFORCEDINLINE auto name([[maybe_unused]] T const_& x __VA_OPT__(           \
      , _detail_ANYXX_MAP_PARAM_LIST_H(a, _sig, __VA_ARGS__)))                 \
      -> decltype(auto) {                                                      \
    return std::visit(                                                         \
        anyxx::overloads{                                                      \
            [&]<typename V>(V&& v) {                                           \
              return x_model_map<std::decay_t<V>>{}.name(                      \
                  std::forward<V>(v) __VA_OPT__(, )                            \
                      __VA_OPT__(_detail_ANYXX_FORWARD_PARAM_LIST(             \
                          a, _sig, __VA_ARGS__)));                             \
            },                                                                 \
            [&]<anyxx::is_any Any>([[maybe_unused]] Any&& any) {               \
              return std::forward<Any>(any).name(__VA_OPT__(                   \
                  _detail_ANYXX_FORWARD_PARAM_LIST(a, _sig, __VA_ARGS__)));    \
            }},                                                                \
        x);                                                                    \
  };

#define _detail_ANYXX_FUNCTION_PTR_DECL(                                   \
    access, overload, type, name, name_ext, exact_const, const_, map_body, \
    mapf_concept, mapf_concept_lhs, ...)                                   \
  anyxx::v_table_return<any_value_t, ANYXX_UNPAREN(type)> (*name##const_)( \
      void const_* __VA_OPT__(                                             \
          , _detail_ANYXX_V_TABLE_PARAM_LIST(a, _sig, __VA_ARGS__)));

#define _detail_ANYXX_LAMBDA_TO_MEMEBER_IMPL(                              \
    access, overload, type, name, name_ext, exact_const, const_, map_body, \
    mapf_concept, mapf_concept_lhs, ...)                                   \
  name##const_ =                                                           \
      [](void const_* _vp __VA_OPT__(                                      \
          , _detail_ANYXX_V_TABLE_PARAM_LIST(a, _sig, __VA_ARGS__)))       \
      -> anyxx::v_table_return<any_value_t, ANYXX_UNPAREN(type)> {         \
    if constexpr (std::same_as<anyxx::self&, ANYXX_UNPAREN(type)>) {       \
      model_map{}.name(                                                    \
          *anyxx::unchecked_unerase_cast<Concrete>(_vp) __VA_OPT__(, )     \
              __VA_OPT__(_detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP(          \
                  a, _sig, __VA_ARGS__)));                                 \
      return anyxx::handle_self_ref_return<ANYXX_UNPAREN(type)>{}();       \
    } else {                                                               \
      return model_map{}.name(                                             \
          *anyxx::unchecked_unerase_cast<Concrete>(_vp) __VA_OPT__(, )     \
              __VA_OPT__(_detail_ANYXX_FORWARD_PARAM_LIST_TO_MAP(          \
                  a, _sig, __VA_ARGS__)));                                 \
    }                                                                      \
  };

#define _detail_ANYXX_V_TABLE_DATA_DECL(type, name, ...) type name;

#define _detail_ANYXX_V_TABLE_DATA_INIT(type, name, ...) name = __VA_ARGS__;

#define _detail_ANYXX_FN_H(l) _detail_ANYXX_FN l
#define _detail_ANYXX_FN(access, overload, type, name, name_ext, exact_const,  \
                         const_, map_body, mapf_concept, mapf_concept_lhs,     \
                         ...)                                                  \
  overload template <typename Self>                                            \
  AYXFORCEDINLINE decltype(auto) name_ext(this Self&& self __VA_OPT__(         \
      , ) __VA_OPT__(_detail_ANYXX_JACKET_PARAM_LIST(a, _sig, __VA_ARGS__)))   \
    requires(::anyxx::const_correct_call_for_proxy_and_self<                   \
             void const_*, typename std::decay_t<Self>::proxy_t,               \
             std::is_const_v<std::remove_reference_t<Self>>, exact_const>)     \
  {                                                                            \
    using self_t = std::decay_t<Self>;                                         \
    using T = typename self_t::T;                                              \
    using proxy_t = typename self_t::proxy_t;                                  \
    using deduced_type = typename self_t::deduced_type;                        \
                                                                               \
    if constexpr (!self_t::is_dyn) {                                           \
      using traited_t = typename self_t::rep_type;                             \
      if constexpr (std::same_as<void, ANYXX_UNPAREN(type)>) {                 \
        return static_dispatch_map_t<T>{}.name(                                \
            get_proxy_value(self) __VA_OPT__(, )                               \
                __VA_OPT__(_detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP(     \
                    a, _sig, __VA_ARGS__)));                                   \
      } else {                                                                 \
        return ANYXX_JACKET_RETURN(type)::forward(                             \
            static_dispatch_map_t<T>{}.name(                                   \
                get_proxy_value(self) __VA_OPT__(, )                           \
                    __VA_OPT__(_detail_ANYXX_FORWARD_JACKET_PARAM_LIST_TO_MAP( \
                        a, _sig, __VA_ARGS__))),                               \
            std::forward<Self>(self));                                         \
      }                                                                        \
    } else {                                                                   \
      if constexpr (std::same_as<void, ANYXX_UNPAREN(type)>) {                 \
        return get_v_table(self)->name##const_(                                \
            anyxx::get_proxy_ptr(std::forward<self_t const_&>(self))           \
                __VA_OPT__(, _detail_ANYXX_FORWARD_PARAM_LIST(a, _sig,         \
                                                              __VA_ARGS__)));  \
      } else {                                                                 \
        return ANYXX_JACKET_RETURN(type)::forward(                             \
            get_v_table(self)->name##const_(                                   \
                anyxx::get_proxy_ptr(std::forward<self_t const_&>(self))       \
                    __VA_OPT__(, _detail_ANYXX_FORWARD_PARAM_LIST(             \
                                     a, _sig, __VA_ARGS__))),                  \
            std::forward<Self>(self));                                         \
      }                                                                        \
    }                                                                          \
  }

#define _detail_ANYXX_CONCEPT_FUNCTIONS(...)                   \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_CONCEPT_FN_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_CONCEPT_STATIC_FUNCTIONS(...)                   \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_CONCEPT_STATIC_FN_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_CONCEPT_TYPES(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_CONCEPT_TYPE_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_MAP_FUNCTIONS(...)                     \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MAP_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_MAP_STATIC_FUNCTIONS(...)                \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MAP_STATIC_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_MAP_TYPES(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MAP_TYPE_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_MAP_VARIANT_FUNCTIONS(...)                     \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MAP_VARIANT_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_V_TABLE_TYPES(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_V_TABLE_TYPE_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_V_TABLE_FUNCTION_PTRS(...)        \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_FPD_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYXX_V_TABLE_LAMBDAS(...)                       \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_MEMEBER_LIMP_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYXX_V_TABLE_DATA_DECLS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_V_TABLE_DATA_DECL_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYXX_V_TABLE_DATA_INITS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_V_TABLE_DATA_INIT_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__));

#define _detail_ANYXX_FNS(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_FN_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_JACKET_STATIC_FNS(...)                      \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_JACKET_STATIC_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_JACKET_TYPES(...)                         \
  __VA_OPT__(_detail_foreach_macro(_detail_ANYXX_JACKET_TYPE_H, \
                                   _detail_EXPAND_LIST __VA_ARGS__))

#define _detail_ANYXX_MAKE_V_TABLE_FUNCTION_NAME(n) \
  _detail_CONCAT(make_, _detail_CONCAT(n, _v_table))

// cppcheck-suppress-macro performance-unnecessary-value-param
#define TRAIT_META_FUNCTION(                                                   \
    any_template_params, model_map_template_params, concrete_template_params,  \
    static_dispatch_template_params, variant_model_map_template_params, n,     \
    BASE, base_template_params, base_model_map_template_params, l, static_fns, \
    typedefs, v_table_data, decoration)                                        \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(any_template_params) struct n;    \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
  struct n##_default_rep;                                                      \
  struct n##_is_nullable;                                                      \
  struct n##_val_size;                                                         \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
  struct n##_default_model_map {                                               \
    using default_map = n##_default_model_map;                                 \
    using rep_type = T;                                                        \
                                                                               \
    struct deduced_type {                                                      \
      _detail_ANYXX_MAP_TYPES(typedefs);                                       \
    };                                                                         \
    _detail_ANYXX_MAP_FUNCTIONS(l);                                            \
    _detail_ANYXX_MAP_STATIC_FUNCTIONS(static_fns);                            \
  };                                                                           \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
  struct n##_model_map : n##_default_model_map<_detail_ANYXX_TEMPLATE_ARGS(    \
                             model_map_template_params)> {                     \
    using rep_type =                                                           \
        anyxx::default_rep<T, n##_default_rep<_detail_ANYXX_TEMPLATE_ARGS(     \
                                  model_map_template_params)>>;                \
  };                                                                           \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params)>      \
    requires(anyxx::is_variant<T>)                                             \
  struct n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(model_map_template_params)> \
      : n##_default_model_map<_detail_ANYXX_TEMPLATE_ARGS(                     \
            model_map_template_params)> {                                      \
    using rep_type = T;                                                        \
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
        _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(                                  \
            base_template_params)::v_table_t {                                 \
    using v_table_base_t = typename BASE _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS( \
        base_template_params)::v_table_t;                                      \
                                                                               \
    using v_table_t = n##_v_table;                                             \
                                                                               \
    using val_nullable =                                                       \
        std::conditional_t<anyxx::is_type_complete<n##_is_nullable>,           \
                           std::true_type,                                     \
                           typename v_table_base_t::val_nullable>;             \
                                                                               \
    static constexpr std::size_t val_proxy_size =                              \
        anyxx::compute_val_proxy_size<n##_val_size>(                           \
            v_table_base_t::val_proxy_size);                                   \
                                                                               \
    using any_value_t = anyxx::any<n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(     \
                                       any_template_params),                   \
                                   anyxx::val<val_nullable, val_proxy_size>>;  \
                                                                               \
    static bool static_is_derived_from(const std::type_info& from) {           \
      if constexpr (anyxx::is_dynamic_castable_v_table<v_table_base_t>) {      \
        return typeid(v_table_t) == from                                       \
                   ? true                                                      \
                   : v_table_base_t::static_is_derived_from(from);             \
      } else {                                                                 \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
                                                                               \
    using T = char;                                                            \
                                                                               \
    struct deduced_type {                                                      \
      _detail_ANYXX_V_TABLE_TYPES(typedefs);                                   \
    };                                                                         \
    _detail_ANYXX_V_TABLE_FUNCTION_PTRS(l);                                    \
    _detail_ANYXX_V_TABLE_DATA_DECLS(v_table_data);                            \
                                                                               \
    template <typename Concrete>                                               \
    explicit(false) n##_v_table(std::in_place_type_t<Concrete> concrete);      \
  };                                                                           \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(any_template_params) struct n     \
      : BASE                                                                   \
        _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(base_template_params) {           \
    using base_t =                                                             \
        BASE _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(base_template_params);       \
                                                                               \
    template <typename T>                                                      \
    using model_map =                                                          \
        n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(model_map_template_params)>; \
                                                                               \
    using val_nullable =                                                       \
        std::conditional_t<anyxx::is_type_complete<n##_is_nullable>,           \
                           std::true_type, typename base_t::val_nullable>;     \
                                                                               \
    static constexpr std::size_t val_proxy_size =                              \
        anyxx::compute_val_proxy_size<n##_val_size>(base_t::val_proxy_size);   \
                                                                               \
    using any_value_t = anyxx::any<n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(     \
                                       any_template_params),                   \
                                   anyxx::val<val_nullable, val_proxy_size>>;  \
                                                                               \
    using v_table_base_t = base_t::v_table_t;                                  \
    using v_table_t =                                                          \
        n##_v_table _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(any_template_params); \
    template <typename StaticDispatchType>                                     \
    using static_dispatch_map_t = n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(   \
        static_dispatch_template_params)>;                                     \
                                                                               \
    template <typename M>                                                      \
    constexpr static bool modeled_by();                                        \
                                                                               \
    _detail_ANYXX_FNS(l);                                                      \
    _detail_ANYXX_JACKET_STATIC_FNS(static_fns);                               \
    _detail_REMOVE_PARENS(decoration);                                         \
  };                                                                           \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(model_map_template_params),      \
            typename deduced_type = n##_model_map<_detail_ANYXX_TEMPLATE_ARGS( \
                model_map_template_params)>::deduced_type>                     \
  concept _detail_CONCAT(_detail_CONCAT(is_, n), _model) =                     \
      requires(                                                                \
          T model,                                                             \
          anyxx::any_trait_class<T, n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(    \
                                        any_template_params)>                  \
              trait_class,                                                     \
          n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(                           \
              model_map_template_params)>                                      \
              model_map) {                                                     \
        requires anyxx::is_type_complete<T>;                                   \
        _detail_ANYXX_CONCEPT_FUNCTIONS(l)                                     \
            _detail_ANYXX_CONCEPT_STATIC_FUNCTIONS(static_fns)                 \
                _detail_ANYXX_CONCEPT_TYPES(typedefs)                          \
      } &&                                                                     \
      BASE _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(                               \
          base_template_params)::template modeled_by<T>();                     \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(                                  \
      any_template_params) template <typename T>                               \
  constexpr bool n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(                       \
      any_template_params)::modeled_by() {                                     \
    return _detail_CONCAT(                                                     \
        _detail_CONCAT(is_, n),                                                \
        _model)<_detail_ANYXX_TEMPLATE_ARGS(model_map_template_params)>;       \
  };                                                                           \
                                                                               \
  _detail_ANYXX_OPTIONAL_TYPENAME_PARAM_LIST(                                  \
      any_template_params) template <typename Concrete>                        \
  n##_v_table                                                                  \
  _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(any_template_params)::n##_v_table(      \
      std::in_place_type_t<Concrete> concrete)                                 \
      : v_table_base_t(concrete) {                                             \
    using model_map =                                                          \
        n##_model_map<_detail_ANYXX_TEMPLATE_ARGS(concrete_template_params)>;  \
                                                                               \
    _detail_ANYXX_V_TABLE_LAMBDAS(l);                                          \
    _detail_ANYXX_V_TABLE_DATA_INITS(v_table_data);                            \
    ::anyxx::set_is_derived_from<v_table_t>(this);                             \
  };

#define __detail_ANYXX_TRAIT_(t, n, BASE, l, static_fns, typedefs,             \
                              v_table_data, decoration)                        \
  TRAIT_META_FUNCTION(, (T), (Concrete), (StaticDispatchType), (V), n, BASE, , \
                      (T), l, static_fns, typedefs, v_table_data, decoration)

/// \addtogroup trait_macros TRAIT... and ANY_ macros
/// \brief Macros to define \ref trait 's and \ref any 's
///
/// Name conventions:
/// - TRAIT: macro to define the functional behavior of an \ref any
/// - ANY: macro to define a \ref trait and a using for an \ref any based on
/// this with a template parameter for the proxy.
/// - EX: macro with decorations (additional functions and typedefs in
/// brackets). A decoration can be any valid C++ code fragment in the context of
/// the trait jacket
/// - TEMPLATE: macro to define a template \ref trait or \ref any
/// - trailing underscore: macro with base TRAIT
///
/// Syntax:
///
/// TRAIT[_TEMPLATE][_EX][_]
///     ([template_types], name, [base, base_template_types,]
///         function_list[,(decoration)])
///
/// ANY[_TEMPLATE][_EX][_]
///     ([template_types], name, [base, base_template_types,]
///         function_list[,(decoration)])
///
///  @{

/// \def TRAIT_EX_
/// \brief TRAIT derived from base with decoration.
/// Macro to define the functional behavior for a \ref any, where the
/// behavior of base is inherited. The decoration are additional functions and
/// typedefs (in brackets).
#define TRAIT_EX_(n, BASE, l, static_fns, typedefs, v_table_data, decoration) \
  __detail_ANYXX_TRAIT_(, n, BASE, l, static_fns, typedefs, v_table_data,     \
                        decoration)

/// \def TRAIT_
/// \brief TRAIT derived from base.
/// \ingroup trait_macros
#define TRAIT_(n, BASE, l) TRAIT_EX_(n, BASE, l, , , , ())

/// \def TRAIT
/// \brief Macro to define the functional behavior for an \ref any.
/// \ingroup trait_macros
///
/// Example:
/// \code
/// TRAIT(sample_trait,
///   (ANY_FN(std::string, const_fn, (double, std::string const&), const))
/// )
/// \endcode
#define TRAIT(n, fns) TRAIT_(n, anyxx::dynamic_copyable, fns)

/// \def TRAIT_EX
/// \brief TRAIT with decoration.
/// \ingroup trait_macros
///
/// Macro to define the functional behavior for a \ref any, with decorations.
/// Decorations are additional functions and typedefs (in brackets).
#define TRAIT_EX(n, l, static_fns, typedefs, v_table_data, decoration)         \
  TRAIT_EX_(n, anyxx::dynamic_copyable, l, static_fns, typedefs, v_table_data, \
            decoration)

/// \def TRAIT_TEMPLATE_EX_
/// \brief TRAIT template with base and decoration.
/// \ingroup trait_macros
///
/// Macro to define the functional behavior for a \ref any, with decorations.
/// Decorations are additional functions and typedefs (in brackets).
#define TRAIT_TEMPLATE_EX_(t, n, base, base_template_types, l, static_fns,     \
                           typedefs, v_table_data, decoration)                 \
  TRAIT_META_FUNCTION(                                                         \
      _detail_REMOVE_PARENS(t),                                                \
      __detail_ANYXX_ADD_HEAD((T), _detail_REMOVE_PARENS(t)),                  \
      __detail_ANYXX_ADD_HEAD((Concrete), _detail_REMOVE_PARENS(t)),           \
      __detail_ANYXX_ADD_HEAD((StaticDispatchType), _detail_REMOVE_PARENS(t)), \
      __detail_ANYXX_ADD_HEAD((V), _detail_REMOVE_PARENS(t)), n, base,         \
      _detail_REMOVE_PARENS(base_template_types),                              \
      __detail_ANYXX_ADD_HEAD((T),                                             \
                              _detail_REMOVE_PARENS(base_template_types)),     \
      l, static_fns, typedefs, v_table_data, decoration)

/// \def TRAIT_TEMPLATE_EX
/// \brief TRAIT template with decoration.
/// \ingroup trait_macros
#define TRAIT_TEMPLATE_EX(t, n, l, static_fns, typedefs, v_table_data, \
                          decoration)                                  \
  TRAIT_TEMPLATE_EX_(t, n, anyxx::dynamic_copyable, (), l, static_fns, \
                     typedefs, v_table_data, decoration)

/// \def TRAIT_TEMPLATE_
/// \brief TRAIT template with a base TRAIT.(
/// \ingroup trait_macros
#define TRAIT_TEMPLATE_(t, n, base, base_template_types, l) \
  TRAIT_TEMPLATE_EX_(t, n, base, base_template_types, l, , , , ())

/// \def TRAIT_TEMPLATE
/// \brief TRAIT template.
/// \ingroup trait_macros
#define TRAIT_TEMPLATE(t, n, l) \
  TRAIT_TEMPLATE_(t, n, anyxx::dynamic_copyable, (), l)

////////////////////////////////////////////////////////////////////////////////
// cppcheck-suppress-macro performance-unnecessary-value-param
#define ANY_META_FUNCTION(pure_template_params,                                \
                          any_template_params_with_defaults, n)                \
                                                                               \
  template <_detail_ANYXX_TYPENAME_PARAM_LIST(                                 \
      any_template_params_with_defaults)>                                      \
  using any_##n =                                                              \
      anyxx::any<n _detail_ANYXX_OPTIONAL_TEMPLATE_ARGS(pure_template_params), \
                 Proxy>;

////////////////////////////////////////////////////////////////////////////////

#define __detail_ANYXX_ANY_CMF(t, t_with_defaults, n) \
  ANY_META_FUNCTION(, _detail_REMOVE_PARENS(t_with_defaults), n)

#define __detail_ANYXX_ANY_EX_(n, proxy_default) \
  __detail_ANYXX_ANY_CMF(                        \
      ((Proxy)), ((Proxy = anyxx::default_proxy<proxy_default>::type)), n)

/// \brief ANY with a base and decoration
/// \ingroup trait_macros
/// See \ref ANY for explanation
#define ANY_EX_(n, BASE, l, proxy_default, decoration) \
  TRAIT_EX_(n, BASE, l, decoration)                    \
  __detail_ANYXX_ANY_EX_(n, proxy_default)

/// \brief ANY with decoration
/// \ingroup trait_macros
/// See \ref ANY for explanation
#define ANY_EX(n, l, proxy_default, decoration) \
  TRAIT_EX(n, l, decoration)                    \
  __detail_ANYXX_ANY_EX_(n, proxy_default)

/// \brief ANY with a base
/// \ingroup trait_macros
/// See \ref ANY for explanation
#define ANY_(n, BASE, l, proxy_default) \
  TRAIT_(n, BASE, l)                    \
  __detail_ANYXX_ANY_EX_(n, proxy_default)

/// \def ANY
/// \brief Simple ANY macro.
///
/// \ingroup trait_macros
/// The ANY macro uses TRAIT macros to define the functional behavior of an \ref
/// any. Additionally, it defines the proxy to be used (default_proxy). The
/// default proxy is val<>. Example:
/// \code
/// ANY(example_any,
///   (ANY_FN(void, example_fn_const, (double, std::string const&), const),
///    ANY_FN(int, example_fn_mutable, (int), ))
/// )
/// \endcode
/// is equivalent to:
/// \code
/// TRAIT(example,
///   (ANY_FN(void, example_fn_const, (double, std::string const&), const),
///    ANY_FN(int, example_fn_mutable, (int), ))
/// )
///
/// template <typename Proxy = anyxx::val<><>>
/// using any_example = anyxx::any<Proxy, example>;
/// \endcode
#define ANY(n, l, ...) \
  TRAIT(n, l)          \
  __detail_ANYXX_ANY_EX_(n, __VA_ARGS__)

#define __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)                 \
  ANY_META_FUNCTION(_detail_REMOVE_PARENS(t),                                \
                    __detail_ANYXX_ADD_TAIL(                                 \
                        (Proxy = anyxx::default_proxy<proxy_default>::type), \
                        _detail_REMOVE_PARENS(t)),                           \
                    n)

/// \def ANY_TEMPLATE_
/// \brief ANY template with a base.
/// \ingroup trait_macros
/// See \ref ANY for explanation
#define ANY_TEMPLATE_EX_(t, n, BASE, bt, l, proxy_default, static_fns,      \
                         typedefs, v_table_data, decoration)                \
  TRAIT_TEMPLATE_EX_(t, n, BASE, bt, l, static_fns, typedefs, v_table_data, \
                     decoration)                                            \
  __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)

/// \def ANY_TEMPLATE_
/// \brief ANY template with a base.
/// \ingroup trait_macros
/// See \ref ANY for explanation
#define ANY_TEMPLATE_(t, n, BASE, bt, l, proxy_default) \
  TRAIT_TEMPLATE_(t, n, BASE, bt, l)                    \
  __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)

/// \def ANY_TEMPLATE
/// \brief ANY template.
/// \ingroup trait_macros
/// See \ref ANY for explanation
#define ANY_TEMPLATE(t, n, l, proxy_default) \
  TRAIT_TEMPLATE(t, n, l)                    \
  __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)

/// \def ANY_TEMPLATE_EX_
/// \brief ANY template with a base and decoration.
/// \ingroup trait_macros
/// See \ref ANY for explanation
#define ANY_TEMPLATE_EX(t, n, l, proxy_default, static_fns, typedefs, \
                        decoration)                                   \
  TRAIT_TEMPLATE_EX(t, n, l, static_fns, typedefs, , decoration)      \
  __detail_ANYXX_ANY_TEMPLATE_CMF(t, n, proxy_default)

/// @}

#define ANY_FN_(...) (__VA_ARGS__)
#define ANY_OVERLOAD(name) using base_t::name;

#define __detail_ANYXX_MEMBER_FN(access, overload, ret, name, name_ext, \
                                 exact_const, const_, params)           \
  ANY_FN_(access, overload, ret, name, name_ext, exact_const, const_,   \
          (x.name_ext), use_mapf_concept, (x.name_ext), _detail_EXPAND params)

/// \addtogroup fn_macros ANY_FN... and ANY_OP macros
/// \brief Macros to define \ref trait's and \ref any's functions and operators
///
/// Name conventions:
/// - FN: function
/// - OP: operator
/// - PURE: function must be provided by the model
/// - DEF: function has a default behavior defined by the last parameter. Must
/// be a lambda. The target model can be accessed via a capture in the lambda of
/// the varaible x. This lambda is the last parameter of the macro.
/// - OVERLOAD: use if in a base TRAIT exists an equally named FN or OP
/// - MAP_NAMED: use to provide a programmer-chosen name for an operator in the
/// map to have an defined name for overriding in derived TRAITs
/// - EXACT: constness of the function must be matched exactly by the proxy.
/// That means, if the function is const, the proxy must be const as well.
/// Useful for functions and operators which have a seperate behavior for const
/// and mutable objects, e.g., operator[].
/// --- no suffix: function whose default behavior is to call an equally named
/// member function of the model
///
/// Some FN/OP forms allow an access specifier. This specifier means
/// - private: this FN/OP must be specified in every model_map. No default
/// available.
/// - protected: this FN/OP must be specified in every model_map. A default
/// implementation is available in ..._default_model_map.
/// - public: this FN/OP can be specified in every model_map. If it is not
/// specified there, then the default implementation from ..._default_model_map
/// is used.
///
/// syntax:
///
/// ANY_FN[_OVERLOAD]([_PURE]|[_DEF])[_EXCACT][]
///     ([access], return_type, name, (param_list),[_const]
///     [, default_behavior])
///
/// ANY_OP[_OVERLOAD][_MAP_NAMED]([_DEF]|[_EXACT_DEF]
///     ([access], return_type, operator [,map_name], (param_list), [_const]
///     [, default_behavior])
///
///  @{

/// \def ANY_FN_PURE
/// \brief TRAIT function, which must be provided by the model.
/// \ingroup trait_macros
#define ANY_FN_PURE(ret, name, params, const_)           \
  ANY_FN_(private, , ret, name, name, false, const_,     \
          (_detail_ANYXX_FN_EMPTY(name, ret)),           \
          ignore_mapf_concept_with_always_false, x.name, \
          _detail_EXPAND params)

/// \def ANY_FN_PURE_EXACT
/// \brief TRAIT function, which must be provided by the model.
/// \ingroup trait_macros
#define ANY_FN_PURE_EXACT(ret, name, params, const_)     \
  ANY_FN_(private, , ret, name, name, true, const_,      \
          (_detail_ANYXX_FN_EMPTY(name, ret)),           \
          ignore_mapf_concept_with_always_false, x.name, \
          _detail_EXPAND params)

/// \def ANY_FN_DEF
/// \brief TRAIT function with default behavior.
/// \ingroup trait_macros
#define ANY_FN_DEF(access, ret, name, params, const_, ...)         \
  ANY_FN_(access, , ret, name, name, false, const_, (__VA_ARGS__), \
          ignore_mapf_concept_with_always_true, x.name, _detail_EXPAND params)

/// \def ANY_FN_DEF_EXACT
/// \brief TRAIT function with default behavior
#define ANY_FN_DEF_EXACT(access, ret, name, params, const_, ...)  \
  ANY_FN_(access, , ret, name, name, true, const_, (__VA_ARGS__), \
          ignore_mapf_concept_with_always_true, x.name, _detail_EXPAND params)

/// \def ANY_FN
/// \brief TRAIT function whose default behavior is to call an equally named
/// member function of the model.
///
/// Example:
/// \code
/// TRAIT(example_trait,
///   (ANY_FN(int, example_fn_const, (double, std::string const&), const),
///    ANY_FN(void, example_fn_mutable, (int), ))
/// )
/// \endcode
#define ANY_FN(ret, name, params, const_) \
  __detail_ANYXX_MEMBER_FN(public, , ret, name, name, false, const_, params)

/// \def ANY_FN_EXACT
/// \brief TRAIT function whose default behavior is to call an equally named
/// member function of the model
/// \ingroup trait_macros
#define ANY_FN_EXACT(ret, name, params, const_) \
  __detail_ANYXX_MEMBER_FN(public, , ret, name, name, true, const_, params)

/// \def ANY_FN_OVERLOAD
/// \brief TRAIT function whose default behavior is to call an equally named
/// member function of the model.
///
/// Use if in a base TRAIT exists an equally named FN.
#define ANY_FN_OVERLOAD(ret, name, params, const_)                             \
  __detail_ANYXX_MEMBER_FN(public, ANY_OVERLOAD(name), ret, name, name, false, \
                           const_, params)

/// \def ANY_FN_OVERLOAD_EXACT
/// \brief TRAIT function whose default behavior is to call an equally named
/// member function of the model.
///
/// Use if in a base TRAIT exists an equally named FN.
#define ANY_FN_OVERLOAD_EXACT(ret, name, params, const_)                      \
  __detail_ANYXX_MEMBER_FN(public, ANY_OVERLOAD(name), ret, name, name, true, \
                           const_, params)

/// \def ANY_OP_MAP_NAMED
/// \brief TRAIT operator with default behavior is to call the related operator
/// of the model and a programmer-chosen name in the map.
///
/// Use if in a base TRAIT exists an equally named FN, or you want to provide a
/// specific name for the operator in the map to have an defined name for
/// overriding in derived TRAITs.
#define ANY_OP_MAP_NAMED(ret, op, name, params, const_)                     \
  __detail_ANYXX_MEMBER_FN(public, , ret, name, operator op, false, const_, \
                           params)

/// \def ANY_FRIENDOP_MAP_NAMED
/// \brief TRAIT operator with default behavior is to call the related friend
/// operator of the model and a programmer-chosen name in the map.
///
/// Use if in a base TRAIT exists an equally named FN, or you want to provide a
/// specific name for the operator in the map to have an defined name for
/// overriding in derived TRAITs.
#define ANY_OP_MAP_NAMED_FRIEND(ret, op, name, params, const_)     \
  ANY_FN_(public, , ret, name, operator op, false, const_, (x op), \
          use_mapf_concept, x op, _detail_EXPAND params)

/// \def ANY_OP
/// \brief TRAIT operator with default behavior is to call the related operator
/// of the model.
#define ANY_OP(ret, op, params, const_) \
  ANY_OP_MAP_NAMED(ret, op, _detail_CONCAT(__op__, __COUNTER__), params, const_)

/// \def ANY_OP_DEF
/// \brief TRAIT operator with default behavior.
///
/// Use if in a base TRAIT exists an equally named FN.
#define ANY_OP_DEF(access, ret, op, name, params, const_, ...)            \
  ANY_FN_(access, , ret, name, operator op, false, const_, (__VA_ARGS__), \
          ignore_mapf_concept_with_always_true, x.operator op,            \
          _detail_EXPAND params)

#define ANY_OP_EXACT_MAP_NAMED(ret, op, name, params, const_)              \
  __detail_ANYXX_MEMBER_FN(public, , ret, name, operator op, true, const_, \
                           params)

/// \def ANY_OP_DEF
/// \brief TRAIT operator with default behavior.
///
/// Use if in a base TRAIT exists an equally named FN.
#define ANY_OP_EXACT(ret, op, params, const_)                                  \
  ANY_OP_EXACT_MAP_NAMED(ret, op, _detail_CONCAT(__op__, __COUNTER__), params, \
                         const_)

/// \def ANY_OP_DEF_EXACT
/// \brief TRAIT operator with default behavior is to call the related operator
/// of the model.
///
/// Use if in a base TRAIT exists an equally named FN.
#define ANY_OP_DEF_EXACT(access, ret, op, name, params, const_, ...)     \
  ANY_FN_(access, , ret, name, operator op, true, const_, (__VA_ARGS__), \
          ignore_mapf_concept_with_always_true, (anyxx::dummy),          \
          _detail_EXPAND params)

/// \def ANY_OP_EXACT_OVERLOAD_MAP_NAMED
/// \brief TRAIT operator with default behavior and a programmer-chosen name
/// in the map.
///
/// Use if in a base TRAIT exists an equally named FN.
#define ANY_OP_EXACT_OVERLOAD_MAP_NAMED(ret, op, name, params, const_)        \
  __detail_ANYXX_MEMBER_FN(ANY_OVERLOAD(operator op), ret, name, operator op, \
                           true, const_, params)

/// \def ANY_OP_EXACT_OVERLOAD
/// \brief TRAIT operator with default behavior is to call the related operator
/// of the model.
///
/// Use if in a base TRAIT exists an equally named FN.
#define ANY_OP_EXACT_OVERLOAD(ret, op, params, const_) \
  ANY_OP_EXACT_OVERLOAD_MAP_NAMED(                     \
      ret, op, _detail_CONCAT(__op__, __COUNTER__), params, const_)

/// \def ANY_OP_EXACT_OVERLOAD_DEF
/// \brief TRAIT operator with default behavior.
///
/// Use if in a base TRAIT exists an equally named FN.
#define ANY_OP_EXACT_OVERLOAD_DEF(access, ret, op, name, params, const_, ...) \
  ANY_FN_(access, ANY_OVERLOAD(operator op), ret, name, operator op, true,    \
          const_, (__VA_ARGS__), ignore_mapf_concept_with_always_true,        \
          x.operator op, _detail_EXPAND params)

/// \def ANY_FN_STATIC_PURE
/// \brief Static TRAIT function, which must be provided by the model. This
/// function will NOT go into the v-Table ad is only avalable for the \ref
/// using_ \ref Proxy.
/// \ingroup trait_macros
#define ANY_FN_STATIC_PURE(template_params, return_type, name, params, ...) \
  ANY_FN_(template_params, return_type, name,                               \
          (_detail_ANYXX_FN_EMPTY(name, return_type)), _detail_EXPAND params)

/// \def ANY_FN_STATIC_DEF
/// \brief Static TRAIT function, which has a default implementation. This
/// function will NOT go into the v-Table and is only avalable for the \ref
/// using_ und \ref trait_class \ref Proxy.
/// \ingroup trait_macros
#define ANY_FN_STATIC_DEF(template_params, return_type, name, params, ...) \
  ANY_FN_(template_params, return_type, name, (__VA_ARGS__),               \
          _detail_EXPAND params)

/// \def ANY_TYPE
/// \brief Dependent type definition in a TRAIT. This is useful for defining
/// associated types, e.g. Return typs for the \ref using_ \ref Proxy.
/// \param template_params template parameters for the type definition
/// \param name name of the type definition
/// \param erased type to be used in the erased context, to simpliy usage.
/// \param default_ default type definition. Used \ref undefined to request
/// specification in the model_map
/// \ingroup trait_macros
#define ANY_TYPE(...) (__VA_ARGS__)

/// \def ANY_V_TABLE_DATA
/// \brief Add a data member to the v-table of a TRAIT. This is useful for
/// meta data, e.g. typeid of wrapped type, a types size or for open dispatch
/// tables.
///
/// \param type type of the data member
/// \param name name of the data member
/// \param erased initializer for the data member. The erased type available as
/// 'Concrete'.
/// \ingroup trait_macros
#define ANY_V_TABLE_DATA(type, name, ...) (type, name, __VA_ARGS__)

/// @}

/// \addtogroup model_map_macros MODEL_MAP macros
/// \brief Macros to define behavior of models for \ref trait's
///
///  @{

#define __ANY_MODEL_MAP(trait_, t)                          \
  template <>                                               \
  struct trait_##_model_map<_detail_ANYXX_TEMPLATE_ARGS(t)> \
      : trait_##_default_model_map<_detail_ANYXX_TEMPLATE_ARGS(t)>

/// \def ANY_TEMPLATE_MODEL_MAP
/// \brief ANY_TEMPLATE_MODEL_MAP macro
/// \param model_ name of the model, including template parameters and namespace
/// in brackets
/// \param trait_ name of the trait, including namespace
/// \param trait_types types to be used as template parameters for the trait
///
/// Must be placed in global namespace, and reachable for the instantiation of
/// the associated v-table.
#define ANY_TEMPLATE_MODEL_MAP(model_, trait_, trait_types) \
  __ANY_MODEL_MAP(trait_, __detail_ANYXX_ADD_HEAD(          \
                              model_, _detail_REMOVE_PARENS(trait_types)))

/// \def ANY_MODEL_MAP
/// \brief ANY_MODEL_MAP macro
/// \param class_ name of the model, including namespace, in brackets
/// \param trait_ name of the trait, including namespace
///
/// Must be placed in global namespace, and reachable for the instantiation of
/// the associated v-table.
#define ANY_MODEL_MAP(model_, trait_) __ANY_MODEL_MAP(trait_, model_)
/// @}

#define _detail_ANYXX_FN_EMPTY(name, ret) \
  []<typename... Args>([[maybe_unused]] Args...) -> ret { return {}; }

