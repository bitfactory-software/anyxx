#pragma once

#include "base.h"

namespace virtual_void::interface {

template <typename BASE_V_TABLE, typename RET, typename... ARGS>
struct call_operator_v_table : BASE_V_TABLE {
  using v_table_base_t = BASE_V_TABLE;
  using void_t = v_table_base_t::void_t;
  using v_table_t = call_operator_v_table;
  static bool static_is_derived_from(const std::type_info& from) {
    return typeid(v_table_t) == from
               ? true
               : BASE_V_TABLE::static_is_derived_from(from);
  }
  RET (*call_op)(void_t, ARGS&&...);
  template <typename UNERASE>
  call_operator_v_table(UNERASE unerase)
      : BASE_V_TABLE(unerase), call_op([](void_t _vp, ARGS&&... args) {
          return (*UNERASE{}(_vp))(std::forward<ARGS>(args)...);
        }) {
    set_is_derived_from<v_table_t>(this);
  }
};

template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          typename CONST, typename RET, typename... ARGS>
struct call_operator_interface;
template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          typename CONST, typename RET, typename... ARGS>
struct call_operator_interface<VIRTUAL_VOID, BASE, CONST, RET(ARGS...)>
    : BASE<VIRTUAL_VOID> {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using void_t = typename virtual_void_trait<VIRTUAL_VOID>::void_t;
  using base_t = BASE<VIRTUAL_VOID>;
  using v_table_base_t = base_t::v_table_t;
  using v_table_t = call_operator_v_table<v_table_base_t, RET, ARGS...>;
  using query_v_table_unique_t =
      call_operator_v_table<base<virtual_void_t>, void>;
  template <typename T>
  using is_already_base =
      std::conditional_t<std::is_same_v<T, query_v_table_unique_t>,
                         std::true_type,
                         typename base_t::template is_already_base<T>>;
  static_assert(!base_t::is_already_base<query_v_table_unique_t>::value,
                "An v_table my only be once in instanciated for a interface");

 protected:
  using base_t::v_table_;
  using base_t::virtual_void_;

 public:
  call_operator_interface(virtual_void_t virtual_void, v_table_t* v_table)
      : base_t(std::move(virtual_void), v_table) {}
  template <typename CONSTRUCTED_WITH>
  call_operator_interface(CONSTRUCTED_WITH&& v)
    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, base_t>)
      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
    static v_table_t imlpemented_v_table{
        unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
    v_table_ = &imlpemented_v_table;
  }
  template <typename OTHER>
  call_operator_interface(const OTHER& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(other) {}
  RET operator()(ARGS&&... args) const
    requires(const_correct_for_virtual_void<CONST, virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)->call_op(
        get_data(base_t::virtual_void_), std::forward<ARGS>(args)...);
  }
  call_operator_interface(const call_operator_interface&) = default;
  call_operator_interface(call_operator_interface&) = default;
  call_operator_interface(call_operator_interface&&) = default;

 protected:
  call_operator_interface() = default;
};
template <typename SIG, is_virtual_void VIRTUAL_VOID,
          template <typename> typename BASE = base>
using call_operator =
    call_operator_interface<VIRTUAL_VOID, BASE, const void*, SIG>;

template <typename SIG, is_virtual_void VIRTUAL_VOID,
          template <typename> typename BASE = base>
using mutable_call_operator =
    call_operator_interface<VIRTUAL_VOID, BASE, void*, SIG>;

};  // namespace virtual_void::interface
