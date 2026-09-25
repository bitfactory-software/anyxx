#pragma once

#include <array>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
#include <bit_factory/v26/trait_translation/overload_set_spec.hpp>
#include <bit_factory/v26/trait_translation/overload_set_spec_with_target.hpp>
#include <bit_factory/v26/trait_translation/get_implementation_member.hpp>
#include <bit_factory/v26/trait_translation/is_defaulted_function_spec.hpp>
#include <bit_factory/v26/trait_translation/find_candidate_in_target.hpp>
#include <bit_factory/v26/any/trait_facade_decorator.hpp>
#include <meta>

namespace anyxx26 {

template <typename V, typename Self>
decltype(auto) self_cast(Self* self){
    return static_cast<V*>(static_cast<std::conditional_t<std::is_const_v<Self>, const void, void>*>(self));
}

template <typename V, std::meta::info Target>
struct const_trait_model_map_call {
  template <typename... Args>
  decltype(auto) operator()(Args&&... args) const {
        return[:Target:](*self_cast<V>(this), std::forward<Args>(args)...);
  }
};
template <typename V, std::meta::info Target>
struct mutable_trait_model_map_call {
    template <typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return[:Target:](*self_cast<V>(this), std::forward<Args>(args)...);
    }
};

template <typename V, std::meta::info Target>
struct const_trait_member_call {
    template <typename... Args>
    decltype(auto) operator()(Args&&... args) const {
        return self_cast<V>(this)->[:Target:](std::forward<Args>(args)...);
    }
};
template <typename V, std::meta::info Target>
struct mutable_trait_member_call {
    template <typename... Args>
    decltype(auto) operator()(Args&&... args) {
        return self_cast<V>(this)->[:Target:](std::forward<Args>(args)...);
    }
};


template <typename V>
consteval std::meta::info make_implementation_call(std::meta::info implementation_member) {
    return implementation_member;

    //if constexpr(is_class_type(^^V)) {
    //    if constexpr(constexpr auto candidate = find_candidate_in_target<spec, V, return_t, VoidSelf, Args...>(); candidate != std::meta::info{}) {
    //        return invoke_member<candidate, self_t, return_t, VoidSelf, Args...>;
    //    }
    //}
    throw std::meta::exception(
        "No implementation found for " +
            std::string{display_string_of(implementation_member)} + " in " +
            std::string{display_string_of(^^V)}, ^^V);
}

template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_static_facade_call(interface_spec_with_target const& spec){
    if (!is_defaulted_function_spec(spec.target)) {
        if(is_const_function(spec.target)) {
            return substitute(^^const_trait_model_map_call, {^^V const, reflect_constant(spec.target)});
        } else {
            return substitute(^^mutable_trait_model_map_call, {^^V, reflect_constant(spec.target)});
        }
    } else {
        if (is_const_function(spec.member)) {
            return substitute(^^const_trait_member_call, {^^V const, reflect_constant(spec.target)});
        } else {
            return substitute(^^mutable_trait_member_call, {^^V, reflect_constant(spec.target)});
        }
    }
    return {};
}

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


}  // namespace anyxx26
