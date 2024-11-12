#pragma once

#include "base.h"
#include "operator.h"
#include "../utillities/type_list.h"

namespace virtual_void::interface {

template<typename RET, typename ARG>
struct subscript_operator_target {
  auto operator()(auto self, ARG arg) const -> RET{
    return (*self)[arg];
  }
};

template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          is_const_specifier CONST_SPECIFIER, typename RET, typename... ARGS>
struct subscript_operator_;
template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          is_const_specifier CONST_SPECIFIER, typename RET, typename... ARGS>
struct subscript_operator_<VIRTUAL_VOID, BASE, CONST_SPECIFIER, RET(ARGS...)>
    : operator_<subscript_operator_target< RET, ARGS...>, VIRTUAL_VOID, BASE, CONST_SPECIFIER,
                RET(ARGS...)> {
  using operator_t = operator_<subscript_operator_target< RET, ARGS...>, VIRTUAL_VOID, BASE,
                               CONST_SPECIFIER, RET(ARGS...)>;
  using operator_t::operator_t;
  using operator_t::operator[];

  RET operator[](first_t<ARGS...> arg) const
    requires(const_correct_for_virtual_void<
             virtual_void::void_t<CONST_SPECIFIER>, VIRTUAL_VOID>)
  {
    return operator_t::invoke(first_t<ARGS...>(arg));
  }
};

template <is_virtual_void VIRTUAL_VOID, typename SIG,
          is_const_specifier CONST_SPECIFIER = const_,
          template <typename> typename BASE = base>
using subscript_operator =
    subscript_operator_<VIRTUAL_VOID, BASE, CONST_SPECIFIER, SIG>;

template <class...>
struct make_overloaded_subscript_operator;

template <class SIG, is_const_specifier CONST_SPECIFIER>
struct make_overloaded_subscript_operator<SIG, CONST_SPECIFIER> {
  template <virtual_void::is_virtual_void VV>
  using type = subscript_operator_<VV, base, CONST_SPECIFIER, SIG>;
};

template <class SIG, is_const_specifier CONST_SPECIFIER, class... SIGS>
struct make_overloaded_subscript_operator<SIG, CONST_SPECIFIER, SIGS...> {
  template <virtual_void::is_virtual_void VV>
  using type = subscript_operator_<
      VV, typename make_overloaded_subscript_operator<SIGS...>::type,
      CONST_SPECIFIER, SIG>;
};

template <virtual_void::is_virtual_void VV, class... SIGS>
using overloaded_subscript_operator =
    make_overloaded_subscript_operator<SIGS...>::template type<VV>;

};  // namespace virtual_void::interface
