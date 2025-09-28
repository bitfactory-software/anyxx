#pragma once

#undef interface

#include <virtual_void/interface/operator.hpp>

namespace virtual_void::interface {

struct call_operator_target {
  template <typename... ARGS>
  decltype(auto) operator()(auto self, ARGS&&... args) const {
    return (*self)(std::forward<ARGS>(args)...);
  }
};

template <is_erased_data ERASED_DATA, template <typename> typename BASE,
          is_constness CONSTNESS, typename RET, typename... ARGS>
struct call_operator_;
template <is_erased_data ERASED_DATA, template <typename> typename BASE,
          is_constness CONSTNESS, typename RET, typename... ARGS>
struct call_operator_<ERASED_DATA, BASE, CONSTNESS, RET(ARGS...)>
    : operator_<call_operator_target, ERASED_DATA, BASE, CONSTNESS,
                RET(ARGS...)> {
  using operator_t = operator_<call_operator_target, ERASED_DATA, BASE,
                               CONSTNESS, RET(ARGS...)>;

  using operator_t::operator_t;

  using operator_t::operator();
  RET operator()(ARGS&&... args) const
    requires(const_correct_call_for_erased_data<virtual_void::void_t<CONSTNESS>,
                                                ERASED_DATA>)
  {
    return operator_t::invoke(std::forward<ARGS>(args)...);
  }
};

template <is_erased_data ERASED_DATA, typename SIG,
          is_constness CONSTNESS = const_,
          template <typename> typename BASE = base>
using call_operator = call_operator_<ERASED_DATA, BASE, CONSTNESS, SIG>;

template <class...>
struct make_overloaded_call_operator;

template <class SIG, is_constness CONSTNESS>
struct make_overloaded_call_operator<SIG, CONSTNESS> {
  template <virtual_void::is_erased_data VV>
  using type = call_operator_<VV, base, CONSTNESS, SIG>;
};

template <class SIG, is_constness CONSTNESS, class... SIGS>
struct make_overloaded_call_operator<SIG, CONSTNESS, SIGS...> {
  template <virtual_void::is_erased_data VV>
  using type =
      call_operator_<VV, typename make_overloaded_call_operator<SIGS...>::type,
                     CONSTNESS, SIG>;
};

template <virtual_void::is_erased_data VV, class... SIGS>
using overloaded_call_operator =
    make_overloaded_call_operator<SIGS...>::template type<VV>;

};  // namespace virtual_void::interface
