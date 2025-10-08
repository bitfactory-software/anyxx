#pragma once

#include <virtual_void/virtual_void.hpp>

namespace virtual_void {

template <typename ARG>
struct translate_erased_function_param {
  using type = ARG;
};
template <is_constness CONSTNESS>
struct translate_erased_function_param<CONSTNESS>
{
  using type = void_t<CONSTNESS>;
};

template <typename RET, typename... ARGS>
struct translate_erased_function {
  using type = RET (*)(typename translate_erased_function_param<ARGS>::type...);
};

}  // namespace virtual_void