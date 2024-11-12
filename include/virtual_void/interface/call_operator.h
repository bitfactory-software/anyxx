#pragma once

#include "base.h"
// #include "operator.h"

namespace virtual_void::interface {

template <typename TARGET, typename BASE_V_TABLE, typename VOID, typename RET,
          typename... ARGS>
struct operator_v_table : BASE_V_TABLE {
  using v_table_base_t = BASE_V_TABLE;
  using void_t = v_table_base_t::void_t;
  using v_table_t = operator_v_table;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : BASE_V_TABLE::static_is_derived_from(from);
  }
  RET (*call_op)(void_t, ARGS&&...);
  template <typename UNERASE>
  operator_v_table(UNERASE unerase) : BASE_V_TABLE(unerase) {
    if constexpr (const_correct_target_for_data<VOID, void_t>) {
      call_op = [](void_t _vp, ARGS&&... args) {
        return (*UNERASE{}(_vp))(std::forward<ARGS>(args)...);
      };
      set_is_derived_from<v_table_t>(this);
    }
  }
};

template <typename TARGET, is_virtual_void VIRTUAL_VOID,
          template <typename> typename BASE, is_const_specifier CONST_SPECIFIER,
          typename RET, typename... ARGS>
struct operator_interface;
template <typename TARGET, is_virtual_void VIRTUAL_VOID,
          template <typename> typename BASE, is_const_specifier CONST_SPECIFIER,
          typename RET, typename... ARGS>
struct operator_interface<TARGET, VIRTUAL_VOID, BASE, CONST_SPECIFIER,
                          RET(ARGS...)> : BASE<VIRTUAL_VOID> {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using void_t = typename virtual_void_trait<VIRTUAL_VOID>::void_t;
  using base_t = BASE<VIRTUAL_VOID>;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t =
      operator_v_table<TARGET, v_table_base_t, CONST_SPECIFIER, RET, ARGS...>;
  using query_v_table_unique_t =
      operator_v_table<TARGET, base<virtual_void_t>,
                       virtual_void::void_t<CONST_SPECIFIER>, RET, ARGS...>;
  template <typename T>
  using is_already_base =
      std::conditional_t<std::is_same_v<T, query_v_table_unique_t>,
                         std::true_type,
                         typename base_t::template is_already_base<T>>;
  static_assert(!base_t::is_already_base<query_v_table_unique_t>::value,
                "A v_table may only instanciated once in an interface");
  using base_t::operator();

 protected:
  using base_t::v_table_;
  using base_t::virtual_void_;

 public:
  operator_interface(virtual_void_t virtual_void, v_table_t* v_table)
      : base_t(std::move(virtual_void), v_table) {}
  template <typename CONSTRUCTED_WITH>
  operator_interface(CONSTRUCTED_WITH&& v)
    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, base_t>)
      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
    static v_table_t imlpemented_v_table{
        unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
    v_table_ = &imlpemented_v_table;
  }
  template <typename OTHER>
  operator_interface(const OTHER& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(other) {}
  // RET operator()(ARGS&&... args) const
  //   requires(const_correct_for_virtual_void<
  //            virtual_void::void_t<CONST_SPECIFIER>, virtual_void_t>)
  //{
  //   return static_cast<v_table_t*>(v_table_)->call_op(
  //       get_data(base_t::virtual_void_), std::forward<ARGS>(args)...);
  // }
  RET invoke(ARGS&&... args) const
    requires(const_correct_for_virtual_void<
             virtual_void::void_t<CONST_SPECIFIER>, virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)->call_op(
        get_data(base_t::virtual_void_), std::forward<ARGS>(args)...);
  }
  operator_interface(const operator_interface&) = default;
  operator_interface(operator_interface&) = default;
  operator_interface(operator_interface&&) = default;

 protected:
  operator_interface() = default;
};

struct call_op_target {
  template <typename... ARGS>
  auto operator()(auto self, ARGS&&... args) const {
    return (*self)(std::forward<ARGS>(args)...);
  }
};

template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          is_const_specifier CONST_SPECIFIER, typename RET, typename... ARGS>
struct call_operator_interface;
template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          is_const_specifier CONST_SPECIFIER, typename RET, typename... ARGS>
struct call_operator_interface<VIRTUAL_VOID, BASE, CONST_SPECIFIER,
                               RET(ARGS...)>
    : operator_interface<call_op_target, VIRTUAL_VOID, BASE, CONST_SPECIFIER,
                         RET(ARGS...)> {
  using operator_interface_t =
      operator_interface<call_op_target, VIRTUAL_VOID, BASE, CONST_SPECIFIER,
                         RET(ARGS...)>;
  using operator_interface_t::operator_interface_t;

  using operator_interface_t::operator();
  RET operator()(ARGS&&... args) const
    requires(const_correct_for_virtual_void<
             virtual_void::void_t<CONST_SPECIFIER>, VIRTUAL_VOID>)
  {
    return operator_interface_t::invoke(std::forward<ARGS>(args)...);
  }
};

template <is_virtual_void VIRTUAL_VOID, typename SIG,
          is_const_specifier CONST_SPECIFIER = const_,
          template <typename> typename BASE = base>
using call_operator =
    call_operator_interface<VIRTUAL_VOID, BASE, CONST_SPECIFIER, SIG>;

template <class...>
struct make_overloaded_call_operator;

template <class SIG, is_const_specifier CONST_SPECIFIER>
struct make_overloaded_call_operator<SIG, CONST_SPECIFIER> {
  template <virtual_void::is_virtual_void VV>
  using type = call_operator_interface<VV, base, CONST_SPECIFIER, SIG>;
};

template <class SIG, is_const_specifier CONST_SPECIFIER, class... SIGS>
struct make_overloaded_call_operator<SIG, CONST_SPECIFIER, SIGS...> {
  template <virtual_void::is_virtual_void VV>
  using type = call_operator_interface<
      VV, typename make_overloaded_call_operator<SIGS...>::type,
      CONST_SPECIFIER, SIG>;
};

template <virtual_void::is_virtual_void VV, class... SIGS>
using overloaded_call_operator =
    make_overloaded_call_operator<SIGS...>::template type<VV>;

};  // namespace virtual_void::interface
