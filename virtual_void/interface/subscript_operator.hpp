#pragma once

#undef interface

#include <virtual_void/interface/operator.hpp>
#include <virtual_void/utillities/type_list.hpp>

namespace virtual_void::interface {

template <typename RET, typename ARG>
struct subscript_operator_target {
  auto operator()(auto self, ARG arg) const -> RET { return (*self)[arg]; }
};

template <data::is_erased_data ERASED_DATA, template <typename> typename BASE,
          is_constness CONSTNESS, typename RET, typename... ARGS>
struct subscript_operator_;
template <data::is_erased_data ERASED_DATA, template <typename> typename BASE,
          is_constness CONSTNESS, typename RET, typename... ARGS>
struct subscript_operator_<ERASED_DATA, BASE, CONSTNESS, RET(ARGS...)>
    : operator_<subscript_operator_target<RET, ARGS...>, ERASED_DATA, BASE,
                CONSTNESS, RET(ARGS...)> {
  using operator_t = operator_<subscript_operator_target<RET, ARGS...>,
                               ERASED_DATA, BASE, CONSTNESS, RET(ARGS...)>;
  using operator_t::operator_t;
  using operator_t::operator[];

  RET operator[](first_t<ARGS...> arg) const
    requires(data::const_correct_call_for_erased_data<CONSTNESS, ERASED_DATA>)
  {
    return operator_t::invoke(first_t<ARGS...>(arg));
  }
};

template <data::is_erased_data ERASED_DATA, typename SIG,
          is_constness CONSTNESS = const_,
          template <typename> typename BASE = base>
using subscript_operator =
    subscript_operator_<ERASED_DATA, BASE, CONSTNESS, SIG>;

template <class...>
struct make_overloaded_subscript_operator;

template <class SIG, is_constness CONSTNESS>
struct make_overloaded_subscript_operator<SIG, CONSTNESS> {
  template <data::is_erased_data VV>
  using type = subscript_operator_<VV, base, CONSTNESS, SIG>;
};

template <class SIG, is_constness CONSTNESS, class... SIGS>
struct make_overloaded_subscript_operator<SIG, CONSTNESS, SIGS...> {
  template <data::is_erased_data VV>
  using type = subscript_operator_<
      VV, typename make_overloaded_subscript_operator<SIGS...>::type, CONSTNESS,
      SIG>;
};

template <data::is_erased_data VV, class... SIGS>
using overloaded_subscript_operator =
    make_overloaded_subscript_operator<SIGS...>::template type<VV>;

};  // namespace virtual_void::interface
