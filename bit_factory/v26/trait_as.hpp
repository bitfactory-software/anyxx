#pragma once

#include <array>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_as/trait_facade.hpp>
#include <bit_factory/v26/trait_translation/trait_facade_decorator.hpp>
#include <meta>

namespace anyxx26 {

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
class trait_as : public[:make_trait_facade<V, Trait, Args...>():], 
    public trait_facade_decorator_t<trait_as<V, Trait, Args...>, Trait, Args...>, 
    public deduced_typenames<Trait, Args...> {
    V value_{};

 public:
     trait_as() = default;
     trait_as(V const& value) : value_(value) {}

    friend V const& get_value(trait_as const& self) { return self.value_; };
    friend V& get_value(trait_as& self) { return self.value_; };
};

template <typename V>
struct using_ {
  template <template <is_trait, typename, typename...> typename Trait, typename... Args>
  using as = trait_as<V, Trait, Args...>;
};

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
auto as(auto&& value){
  return trait_as<std::remove_reference_t<decltype(value)>, Trait, Args...>(std::forward<decltype(value)>(value));
}

#define __dyn_OP_CONST(function, op) \
template <template <typename, typename, typename...> typename Trait, typename V, typename Other, typename... Args> \
    requires (requires(trait_as<V, Trait, Args...> const& lhs, Other const& rhs){ {lhs.function(rhs)}; }) \
decltype(auto) operator op (trait_as<V, Trait, Args...> const& lhs, Other const& rhs) { \
    return lhs.function(rhs); \
}
#define __dyn_OP_MUTATING(function, op) \
template <template <typename, typename, typename...> typename Trait, typename V, typename Other, typename... Args> \
    requires (requires(trait_as<V, Trait, Args...>& lhs, Other const& rhs){ {lhs.function(rhs)}; }) \
decltype(auto) operator op (trait_as<V, Trait, Args...>& lhs, Other const& rhs) { \
    return lhs.function(rhs); \
}
#define __dyn_OP0(function, op) \
template <template <typename, typename, typename...> typename Trait, typename V, typename... Args> \
    requires (requires(trait_as<V, Trait, Args...> const& lhs){ {lhs.function()}; }) \
decltype(auto) operator op (trait_as<V, Trait, Args...> const& lhs) { \
    return lhs.function(); \
}
#define __dyn_OP0_MUTATING(function, op) \
template <template <typename, typename, typename...> typename Trait, typename V, typename... Args> \
    requires (requires(trait_as<V, Trait, Args...>& lhs){ {lhs.function()}; }) \
decltype(auto) operator op (trait_as<V, Trait, Args...>& lhs) { \
    return lhs.function(); \
}

__dyn_OP_CONST(op_equals_equals, == )
__dyn_OP_CONST(op_exclamation_equals, != )
__dyn_OP_CONST(op_less, < )
__dyn_OP_CONST(op_greater, > )
__dyn_OP_CONST(op_less_equals, <= )
__dyn_OP_CONST(op_greater_equals, >= )
__dyn_OP_CONST(op_spaceship, <=> )

__dyn_OP0(op_tilde, ~)
__dyn_OP0(op_exclamation, !)
__dyn_OP0(op_plus, +)
__dyn_OP0(op_minus, -)
__dyn_OP0(op_star, *)
__dyn_OP0_MUTATING(op_star, *)

__dyn_OP_CONST(op_plus, +)
__dyn_OP_CONST(op_minus, -)
__dyn_OP_CONST(op_star, *)
__dyn_OP_CONST(op_slash, / )
__dyn_OP_CONST(op_percent, %)
__dyn_OP_CONST(op_caret, ^)
__dyn_OP_CONST(op_pipe, | )

__dyn_OP_MUTATING(op_plus_equals, +=)
__dyn_OP_MUTATING(op_minus_equals, -=)
__dyn_OP_MUTATING(op_star_equals, *=)
__dyn_OP_MUTATING(op_slash_equals, /=)
__dyn_OP_MUTATING(op_percent_equals, %=)
__dyn_OP_MUTATING(op_caret_equals, ^=)
__dyn_OP_MUTATING(op_ampersand_equals, &=)
__dyn_OP_MUTATING(op_pipe_equals, |=)
__dyn_OP_MUTATING(op_less_less, << )
__dyn_OP_MUTATING(op_greater_greater, >> )
__dyn_OP_MUTATING(op_less_less_equals, <<=)
__dyn_OP_MUTATING(op_greater_greater_equals, >>=)

#undef __dyn_OP_CONST
#undef __dyn_OP_MUTATING
#undef __dyn_OP0
#undef __dyn_OP0_MUTATING

}  // namespace anyxx26
