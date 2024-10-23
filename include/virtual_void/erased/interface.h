#pragma once

//
// developed from:
// https://github.com/AlexCodesApps/virtual_void::erased/blob/main/dynamic_interface.hpp
//
// for Microsoft C++, you must enable the C-Preprocessor with this flag:
// /Zc:preprocessor (see CMakeLists.txt for example)
//

#include <optional>
#include <type_traits>
#include <typeinfo>

#include "concept.h"

namespace virtual_void::erased {

template <typename VOID>
struct interface_base {
  using void_t = VOID;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(interface_base) == from;
  }
  bool (*_is_derived_from)(const std::type_info&);
  template <typename UNERASE>
  interface_base(UNERASE)
      : _is_derived_from([](const std::type_info& from) {
          return static_is_derived_from(from);
        }){};
};

template <is_virtual_void VIRTUAL_VOID>
class base {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using interface_t = interface_base<typename VIRTUAL_VOID::void_t>;
  template <typename>
  using is_already_base =
      std::false_type;  // base is always at the bottom of the interface chain.
 protected:
  virtual_void_t lifetime_holder_ = nullptr;
  interface_t* interface_impementation_ = nullptr;

 public:
  base(virtual_void_t lifetime_holder, interface_t* v_table)
      : lifetime_holder_(std::move(lifetime_holder)),
        interface_impementation_(v_table) {}
  template <typename CONSTRUCTED_WITH>
  base(CONSTRUCTED_WITH&& constructed_with)
    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>,
                                base<VIRTUAL_VOID>>)
      : lifetime_holder_(virtual_void::erased::erase_to<virtual_void_t>(
            std::forward<CONSTRUCTED_WITH>(constructed_with))) {
    static interface_t imlpemented_interface{
        virtual_void::erased::unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
    interface_impementation_ = &imlpemented_interface;
  }
  template <typename OTHER>
  base(const OTHER& other)
    requires(std::derived_from<OTHER, base<VIRTUAL_VOID>>)
      : lifetime_holder_(*other),
        interface_impementation_(other.get_interface()) {}
  base(const base&) = default;
  base(base&) = default;
  base(base&&) = default;
  auto& operator*() const { return lifetime_holder_; }
  auto& operator*() { return lifetime_holder_; }
  interface_t* get_interface() const { return interface_impementation_; }
  bool is_derived_from(const std::type_info& from) const {
    return interface_impementation_->_is_derived_from(from);
  }
  template <typename FROM>
  bool is_derived_from() const {
    return is_derived_from(typeid(FROM::interface_t));
  }

 protected:
  base() = default;
};

template <typename V_TABLE>
void set_is_derived_from(auto interface) {
  interface->_is_derived_from = +[](const std::type_info& from) {
    return V_TABLE::static_is_derived_from(from);
  };
}

template <typename TO, typename FROM>
TO static_interface_cast(const FROM& from)
  requires(std::derived_from<TO, FROM>)
{
  return *static_cast<const TO*>(&from);
}
template <typename TO, typename FROM>
std::optional<TO> interface_cast(const FROM& from)
  requires(std::derived_from<TO, FROM>)
{
  if (from.is_derived_from<TO>()) return {*static_cast<const TO*>(&from)};
  return {};
}

template <typename TO, typename FROM>
TO interface_lifetime_cast(const FROM& from) {
  return TO(
      lifetime_cast<typename TO::virtual_void_t>(*from),
      v_table_cast<TO>(from.get_interface()));
}

}  // namespace virtual_void::erased

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
#define _detail_INTERFACE_MAP_LIMP_H(l) _detail_INTERFACE_MAP_IMPL l
#define _detail_INTERFACE_MEMEBER_LIMP_H(l) \
  _detail_INTERFACE_LAMBDA_TO_MEMEBER_IMPL l
#define _detail_INTERFACE_FREE_LIMP_H(l) _detail_INTERFACE_LAMBDA_TO_FREE_IMPL l
#define _detail_INTERFACE_METHOD_H(l) _detail_INTERFACE_METHOD l
#define _detail_LEAD_COMMA_H_E(l) _detail_LEAD_COMMA_H l

#define _detail_INTERFACE_MAP_IMPL(type, name, const_, ...)                 \
  auto name(T const_* x __VA_OPT__(, _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) { \
    return x->name(__VA_OPT__(_detail_PARAM_LIST(a, _sig, __VA_ARGS__)));   \
  };

#define _detail_INTERFACE_FUNCTION_PTR_DECL(type, name, const_, ...) \
  type (*name)(void_t __VA_OPT__(, __VA_ARGS__));

#define _detail_INTERFACE_LAMBDA_TO_MEMEBER_IMPL(type, name, const_, ...)      \
  name =                                                                       \
      [](void_t _vp __VA_OPT__(, _detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) { \
        return interface_map{}.name((UNERASE{}(_vp))__VA_OPT__(, ) __VA_OPT__( \
            _detail_PARAM_LIST(a, _sig, __VA_ARGS__)));                        \
      };

#define _detail_INTERFACE_METHOD(type, name, const_, ...)                   \
  type name(__VA_OPT__(_detail_PARAM_LIST2(a, _sig, __VA_ARGS__))) const_   \
    requires(virtual_void::erased::const_correct_for_lifetime_holder<       \
             void const_, virtual_void_t>)                              \
  {                                                                         \
    return static_cast<interface_t*>(interface_impementation_)              \
        ->name(base_t::lifetime_holder_.data()                              \
                   __VA_OPT__(, _detail_PARAM_LIST(a, _sig, __VA_ARGS__))); \
  }

#define ERASED_INTERFACE_(n, BASE, l)                                          \
  template <typename T>                                                        \
  struct n##_default_interface_map {                                           \
    _detail_foreach_macro(_detail_INTERFACE_MAP_LIMP_H, _detail_EXPAND_LIST l) \
  };                                                                           \
  template <typename T>                                                        \
  struct n##_interface_map : n##_default_interface_map<T> {};                  \
                                                                               \
  template <typename BASE_V_TABLE>                                             \
  struct n##interface : BASE_V_TABLE {                                         \
    using interface_base_t = BASE_V_TABLE;                                     \
    using void_t = interface_base_t::void_t;                                   \
    using v_table_t = n##interface;                                            \
    static bool static_is_derived_from(const std::type_info& from) {           \
      return typeid(v_table_t) == from                                         \
                 ? true                                                        \
                 : interface_base_t::static_is_derived_from(from);             \
    }                                                                          \
                                                                               \
    _detail_foreach_macro(_detail_INTERFACE_FPD_H, _detail_EXPAND_LIST l)      \
                                                                               \
        template <typename UNERASE>                                            \
        n##interface(UNERASE unerase) : interface_base_t(unerase) {            \
      using interface_map = n##_interface_map<typename UNERASE::type>;         \
      _detail_foreach_macro(_detail_INTERFACE_MEMEBER_LIMP_H,                  \
                            _detail_EXPAND_LIST l);                            \
      virtual_void::erased::set_is_derived_from<v_table_t>(this);              \
    };                                                                         \
  };                                                                           \
                                                                               \
  template <virtual_void::erased::is_virtual_void VIRTUAL_VOID>   \
  struct n : BASE<VIRTUAL_VOID> {                                           \
   public:                                                                     \
    using virtual_void_t = VIRTUAL_VOID;                                 \
    using void_t = VIRTUAL_VOID::void_t;                                    \
    using base_t = BASE<VIRTUAL_VOID>;                                      \
    using interface_base_t = base_t::interface_t;                              \
    using interface_t = n##interface<interface_base_t>;                        \
    using query_interface_unique_t =                                           \
        n##interface<virtual_void::erased::base<virtual_void_t>>;           \
    template <typename T>                                                      \
    using is_already_base =                                                    \
        std::conditional_t<std::is_same_v<T, query_interface_unique_t>,        \
                           std::true_type,                                     \
                           typename base_t::template is_already_base<T>>;      \
    static_assert(                                                             \
        !base_t::is_already_base<query_interface_unique_t>::value,             \
        "An interface my only be once in instanciated for a facade");          \
                                                                               \
   protected:                                                                  \
    using base_t::lifetime_holder_;                                            \
    using base_t::interface_impementation_;                                    \
                                                                               \
   public:                                                                     \
    n(virtual_void_t lifetime_holder, interface_t* v_table)                 \
        : base_t(std::move(lifetime_holder), v_table) {}                       \
    template <typename CONSTRUCTED_WITH>                                       \
    n(CONSTRUCTED_WITH&& v)                                                    \
      requires(                                                                \
          !std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, base_t>)   \
        : base_t(std::forward<CONSTRUCTED_WITH>(v)) {                          \
      static interface_t imlpemented_interface{                                \
          virtual_void::erased::unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()}; \
      interface_impementation_ = &imlpemented_interface;                       \
    }                                                                          \
    template <typename OTHER>                                                  \
    n(const OTHER& other)                                                      \
      requires(std::derived_from<OTHER, base_t>)                               \
        : base_t(other) {}                                                     \
    _detail_foreach_macro(_detail_INTERFACE_METHOD_H, _detail_EXPAND_LIST l)   \
        n(const n&) = default;                                                 \
    n(n&) = default;                                                           \
    n(n&&) = default;                                                          \
                                                                               \
   protected:                                                                  \
    n() = default;                                                             \
  };
#define ERASED_INTERFACE(name, l) \
  ERASED_INTERFACE_(name, virtual_void::erased::base, l)
#define INTERFACE_METHOD_(...) (__VA_ARGS__)
#define INTERFACE_METHOD(ret, name, ...) \
  INTERFACE_METHOD_(ret, name, , __VA_ARGS__)
#define INTERFACE_CONST_METHOD(ret, name, ...) \
  INTERFACE_METHOD_(ret, name, const, __VA_ARGS__)

