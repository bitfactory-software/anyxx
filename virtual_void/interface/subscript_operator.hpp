#pragma once

#undef interface 

#include <virtual_void/utillities/type_list.hpp>
#include <virtual_void/interface/operator.hpp>

namespace virtual_void::interface {

template<typename RET, typename ARG>
struct subscript_operator_target {
  auto operator()(auto self, ARG arg) const -> RET{
    return (*self)[arg];
  }
};

template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          constness CONSTNESS, typename RET, typename... ARGS>
struct subscript_operator_;
template <is_virtual_void VIRTUAL_VOID, template <typename> typename BASE,
          constness CONSTNESS, typename RET, typename... ARGS>
struct subscript_operator_<VIRTUAL_VOID, BASE, CONSTNESS, RET(ARGS...)>
    : operator_<subscript_operator_target< RET, ARGS...>, VIRTUAL_VOID, BASE, CONSTNESS,
                RET(ARGS...)> {
  using operator_t = operator_<subscript_operator_target< RET, ARGS...>, VIRTUAL_VOID, BASE,
                               CONSTNESS, RET(ARGS...)>;
  using operator_t::operator_t;
  using operator_t::operator[];

  RET operator[](first_t<ARGS...> arg) const
    requires(const_correct_for_virtual_void<
             virtual_void::void_t<CONSTNESS>, VIRTUAL_VOID>)
  {
    return operator_t::invoke(first_t<ARGS...>(arg));
  }
};

template <is_virtual_void VIRTUAL_VOID, typename SIG,
          constness CONSTNESS = const_,
          template <typename> typename BASE = base>
using subscript_operator =
    subscript_operator_<VIRTUAL_VOID, BASE, CONSTNESS, SIG>;

template <class...>
struct make_overloaded_subscript_operator;

template <class SIG, constness CONSTNESS>
struct make_overloaded_subscript_operator<SIG, CONSTNESS> {
  template <virtual_void::is_virtual_void VV>
  using type = subscript_operator_<VV, base, CONSTNESS, SIG>;
};

template <class SIG, constness CONSTNESS, class... SIGS>
struct make_overloaded_subscript_operator<SIG, CONSTNESS, SIGS...> {
  template <virtual_void::is_virtual_void VV>
  using type = subscript_operator_<
      VV, typename make_overloaded_subscript_operator<SIGS...>::type,
      CONSTNESS, SIG>;
};

template <virtual_void::is_virtual_void VV, class... SIGS>
using overloaded_subscript_operator =
    make_overloaded_subscript_operator<SIGS...>::template type<VV>;

};  // namespace virtual_void::interface
