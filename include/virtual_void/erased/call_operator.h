#pragma once

#include "interface.h"

namespace virtual_void::erased {

template <typename BASE_V_TABLE, typename RET, typename... ARGS>
struct call_operator_v_table : BASE_V_TABLE {
  using interface_base_t = BASE_V_TABLE;
  using void_t = interface_base_t::void_t;
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
struct call_operator_facade;
template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          typename CONST, typename RET, typename... ARGS>
struct call_operator_facade<VIRTUAL_VOID, BASE, CONST, RET(ARGS...)>
    : BASE<VIRTUAL_VOID> {
 public:
  using virtual_void_t = VIRTUAL_VOID;
  using void_t = VIRTUAL_VOID::void_t;
  using base_t = BASE<VIRTUAL_VOID>;
  using interface_base_t = base_t::v_table_t;
  using v_table_t = call_operator_v_table<interface_base_t, RET, ARGS...>;
  using query_interface_unique_t =
      call_operator_v_table<virtual_void::erased::base<virtual_void_t>, void>;
  template <typename T>
  using is_already_base =
      std::conditional_t<std::is_same_v<T, query_interface_unique_t>,
                         std::true_type,
                         typename base_t::template is_already_base<T>>;
  static_assert(!base_t::is_already_base<query_interface_unique_t>::value,
                "An interface my only be once in instanciated for a facade");

 protected:
  using base_t::v_table_;
  using base_t::virtual_void_;

 public:
  call_operator_facade(virtual_void_t virtual_void, v_table_t* v_table)
      : base_t(std::move(virtual_void), v_table) {}
  template <typename CONSTRUCTED_WITH>
  call_operator_facade(CONSTRUCTED_WITH&& v)
    requires(!std::derived_from<std::remove_cvref_t<CONSTRUCTED_WITH>, base_t>)
      : base_t(std::forward<CONSTRUCTED_WITH>(v)) {
    static v_table_t imlpemented_interface{
        virtual_void::erased::unerase<VIRTUAL_VOID, CONSTRUCTED_WITH>()};
    v_table_ = &imlpemented_interface;
  }
  template <typename OTHER>
  call_operator_facade(const OTHER& other)
    requires(std::derived_from<OTHER, base_t>)
      : base_t(other) {}
  RET operator()(ARGS&&... args) const
    requires(virtual_void::erased::const_correct_for_lifetime_holder<
             CONST, virtual_void_t>)
  {
    return static_cast<v_table_t*>(v_table_)
        ->call_op(base_t::virtual_void_.data(), std::forward<ARGS>(args)...);
  }
  call_operator_facade(const call_operator_facade&) = default;
  call_operator_facade(call_operator_facade&) = default;
  call_operator_facade(call_operator_facade&&) = default;

 protected:
  call_operator_facade() = default;
};
template <typename SIG, is_virtual_void VIRTUAL_VOID,
          template <typename> typename BASE = base>
using call_operator = call_operator_facade<VIRTUAL_VOID, BASE, const void, SIG>;

template <typename SIG, is_virtual_void VIRTUAL_VOID,
          template <typename> typename BASE = base>
using mutable_call_operator =
    call_operator_facade<VIRTUAL_VOID, BASE, void, SIG>;

};  // namespace virtual_void::erased
