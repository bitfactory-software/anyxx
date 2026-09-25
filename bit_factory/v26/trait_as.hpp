#pragma once

#include <array>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
#include <bit_factory/v26/trait_translation/overload_set_spec.hpp>
#include <bit_factory/v26/trait_translation/overload_set_spec_with_target.hpp>
#include <bit_factory/v26/trait_translation/get_implementation_member.hpp>
#include <bit_factory/v26/trait_translation/is_defaulted_function_spec.hpp>
#include <bit_factory/v26/trait_translation/find_candidate_in_target.hpp>
#include <bit_factory/v26/trait_as/trait_calls.hpp>
#include <bit_factory/v26/any/trait_facade_decorator.hpp>
#include <meta>

namespace anyxx26 {

template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info static_facade_named_overload_set(overload_set_spec_with_target const& spec){
    std::vector<std::meta::info> overload_set;
    for(auto overload : std::define_static_array(spec.specs)) {
        if(auto call = make_static_facade_call<V, Trait, Args...>(overload); call != std::meta::info{}) {
            overload_set.push_back(call);
        }
    }
    auto overloaded_call_operator = substitute(^^meta::overload, overload_set);
    return std::meta::data_member_spec(overloaded_call_operator, { .name = spec.name, .no_unique_address = true });
}

template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
consteval auto make_static_facade_overloaded_calls() {
    std::vector<std::meta::info> calls;
    for(auto const& set : make_overload_sets_specs_with_target<V, Trait, Args...>()) {
        calls.push_back(reflect_constant(static_facade_named_overload_set<V, Trait, Args...>(set)));
    }
    return calls;
};

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_trait_facade() {
    return substitute(^^meta::to_struct, make_static_facade_overloaded_calls<V, Trait, Args...>());
};

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
class trait_as : public[:make_trait_facade<V, Trait, Args...>():], public trait_facade_decorator_t<Trait, Args...> {
  V value_;

 public:
  trait_as(V const& value) : value_(value) {}
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
