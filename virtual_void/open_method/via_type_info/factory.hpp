#pragma once

#include <virtual_void/open_method/via_type_info/declaration_base.hpp>

namespace virtual_void::open_method::via_type_info {

template <typename R, typename... ARGS>
class factory;

template <typename R, typename... ARGS>
class factory<R(ARGS...)> : public declaration_base {
 public:
  using factory_function_t = R (*)(ARGS...);
  declaration_base::declaration_base;
  using result_t = R;

 public:
  template <typename CLASS, typename FACTORY>
  auto define(FACTORY f) {
    auto fp = ensure_factory_ptr<CLASS>(f);
    return define_erased(typeid(CLASS),
                         reinterpret_cast<factory_function_t>(fp));
  }
  R operator()(const std::type_info& type_info, ARGS&&... args) const {
    auto f = lookup<factory_function_t>(type_info);
    return f(std::forward<ARGS>(args)...);
  }
  template <typename CLASS>
  R construct(ARGS&&... args) const  // to simplify tests!
  {
    return (*this)(typeid(CLASS), std::forward<ARGS>(args)...);
  }
  template <typename CLASS>
  R operator()(ARGS&&... args) const  // to simplify tests!
  {
    return construct<CLASS>(std::forward<ARGS>(args)...);
  }

 private:
  template <typename CLASS>
  static auto ensure_factory_ptr(
      auto functor)  // if functor is a templated operator() from a stateless
                     // function object, instantiate it now!;
  {
    using functor_t = decltype(functor);
    if constexpr (std::is_pointer_v<functor_t>) {
      return functor;
    } else {
      return +[](ARGS&&... args) -> R {
        return functor_t{}.template operator()<CLASS>(
            std::forward<ARGS>(args)...);
      };
    }
  }
};

}  // namespace virtual_void::typeid_