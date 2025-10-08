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

template <typename CLASS1, template <typename> typename PARAM1, typename CLASS2, template <typename> typename PARAM2, typename R,
          typename DISPATCH1, typename DISPATCH2, typename... OTHER_ARGS>
auto ensure_function_ptr2(
    auto functor)  // if functor is a templated operator() from a stateless
                   // function object, instantiate it now!;
{
  using functor_t = decltype(functor);
  if constexpr (std::is_pointer_v<functor_t>) {
    return functor;
  } else {
    return +[](PARAM1<CLASS1> p1, PARAM1<CLASS2> p2, OTHER_ARGS... args) -> R {
      return functor_t{}(p1, p2, args...);
    };
  }
}

}  // namespace virtual_void
