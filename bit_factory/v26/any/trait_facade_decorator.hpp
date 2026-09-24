#pragma once

#include <bit_factory/v26/any/keywords.hpp>

namespace anyxx26 {

struct no_trait_facade_decorator_t {};

struct default_operator_facade {

#define __dyn_OP(function, op) \
  template <typename Self, typename... Params> \
  decltype(auto) operator op (this Self&& self, Params&&... params) { \
      return self.function(std::forward<Params>(params)...); \
  }
#define __dyn_OP0(function, op) \
  template <typename Self> \
  decltype(auto) operator op (this Self&& self) { \
      return self.function(); \
  }

    __dyn_OP(op_parentheses, ())
    __dyn_OP(op_square_brackets, [])
    __dyn_OP0(op_arrow, ->)
    __dyn_OP0(op_plus_plus, ++)
    __dyn_OP0(op_minus_minus, --)
     //    __dyn_OP(op_ampersand, &)

#undef __dyn_OP
#undef __dyn_OP0

        template <typename Self>
    decltype(auto) operator++(this Self&& self, int) {
        std::decay_t<Self> old = self;
        ++self;
        return old;
    }
    template <typename Self>
    decltype(auto) operator--(this Self&& self, int) {
        std::decay_t<Self> old = self;
        --self;
        return old;
    }

};

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
concept has_trait_facade_decorator = requires(no_trait_facade_decorator_t){
  typename trait_declaration_t<Trait, Args...>::template trait_facade_decorator<no_trait_facade_decorator_t>;
};
template <typename DummySelf, template <is_trait, typename, typename...> typename Trait, typename... Args>
concept has_trait_facade_decorator_for_self = has_trait_facade_decorator<Trait, Args...>;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info trait_facade_decorator() {
  if constexpr (has_trait_facade_decorator<Trait, Args...>) {
    return ^^typename anyxx26::trait_declaration_t<Trait, Args...>:: template trait_facade_decorator<any<Trait, Args...>>;
  } else {
    return ^^default_operator_facade;
  }
}

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using trait_facade_decorator_t = [:trait_facade_decorator<Trait, Args...>():];

}  // namespace anyxx26

