#pragma once

#include "base.h"
#include "operator.h"

namespace virtual_void::interface {

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
