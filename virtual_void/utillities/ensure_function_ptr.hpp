#pragma once

namespace virtual_void {

template <typename CLASS, template <typename> typename PARAM, typename R,
          typename DISPATCH, typename... OTHER_ARGS>
using function_ptr_t = auto(*)(PARAM<CLASS>, OTHER_ARGS...) -> R;

template <typename CLASS, template <typename> typename PARAM, typename R,
          typename DISPATCH, typename... OTHER_ARGS>
auto ensure_function_ptr(
    auto functor)  // if functor is a templated operator() from a stateless
                   // function object, instantiate it now!;
{
  using functor_t = decltype(functor);
  if constexpr (std::is_pointer_v<functor_t>) {
    return functor;
  } else {
    return +[](PARAM<CLASS> self, OTHER_ARGS... args) -> R {
      return functor_t{}(self, args...);
    };
  }
}

}  // namespace virtual_void
