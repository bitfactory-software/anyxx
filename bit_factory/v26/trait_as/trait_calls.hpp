#pragma once

#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
#include <bit_factory/v26/trait_as/signature_translation.hpp>

namespace anyxx26 {

template <typename V, typename Self>
decltype(auto) self_cast(Self* self){
    return static_cast<V*>(static_cast<std::conditional_t<std::is_const_v<Self>, const void, void>*>(self));
}

template <typename V, std::meta::info Target>
struct const_trait_model_map_call {
  template <typename Self, typename... Args>
  decltype(auto) operator()(this Self const& self, Args&&... args){
        return[:Target:](*self_cast<V>(&self), std::forward<Args>(args)...);
  }
};
template <typename V, std::meta::info Target>
struct mutable_trait_model_map_call {
    template <typename Self, typename... Args>
    decltype(auto) operator()(this Self&  self, Args&&... args) {
        return[:Target:](*self_cast<V>(&self), std::forward<Args>(args)...);
    }
};

template <typename V, std::meta::info Target>
struct const_trait_member_call {
    template <typename Self, typename... Args>
    decltype(auto) operator()(this Self const& self, Args&&... args) {
        return self_cast<V>(&self)->[:Target:](std::forward<Args>(args)...);
    }
};
template <typename V, std::meta::info Target>
struct mutable_trait_member_call {
    template <typename Self, typename... Args>
    decltype(auto) operator()(this Self&  self, Args&&... args) {
        return self_cast<V>(&self)->[:Target:](std::forward<Args>(args)...);
    }
};

#define __DEFINE_TRAIT_INVOKE_OP(constness, const_) \
  template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_parentheses { \
    template <typename Self, typename... Args> \
    decltype(auto) operator()(this Self const_& self, Args&&... args) { \
        return (*self_cast<V>(&self))(std::forward<Args>(args)...); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_plus_plus { \
    template <typename Self, typename... Args> \
    decltype(auto) operator()(this Self const_& self, Args&&... args) { \
        return ++(*self_cast<V>(&self)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_minus_minus { \
    template <typename Self, typename... Args> \
    decltype(auto) operator()(this Self const_& self, Args&&... args) { \
        return --(*self_cast<V>(&self)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_star { \
    template <typename Self, typename... Args> \
    decltype(auto) operator()(this Self const_& self, Args&&... args) { \
        if constexpr(sizeof...(Args) == 0) { \
            return *(*self_cast<V>(&self)); \
        } else { \
            return ((*self_cast<V>(&self)) * ... * std::forward<Args>(args)); \
        } \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_arrow { \
    template <typename Self, typename... Args> \
    decltype(auto) operator()(this Self const_& self, Args&&... args) { \
        return std::to_address(*self_cast<V>(&self)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_square_brackets { \
    template <typename Self, typename... Args> \
    decltype(auto) operator()(this Self const_& self, Args&&... args) { \
        return (*self_cast<V>(&self))[std::forward<Args>(args)...]; \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_plus { \
    template <typename Self, typename... Args> \
    decltype(auto) operator()(this Self const_& self, Args&&... args) { \
        if constexpr(sizeof...(Args) == 0) { \
            return +(*self_cast<V>(&self)); \
        } else { \
            return ((*self_cast<V>(&self)) + ... + std::forward<Args>(args)); \
        } \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_minus { \
    template <typename Self, typename... Args> \
    decltype(auto) operator()(this Self const_& self, Args&&... args) { \
        if constexpr(sizeof...(Args) == 0) { \
            return -(*self_cast<V>(&self)); \
        } else { \
            return ((*self_cast<V>(&self)) - ... - std::forward<Args>(args)); \
        } \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_equals_equals { \
    template <typename Self, typename Arg> \
    decltype(auto) operator()(this Self const_& self, Arg&& arg) { \
        return ((*self_cast<V>(&self)) == std::forward<Arg>(arg)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_exclamation_equals { \
    template <typename Self, typename Arg> \
    decltype(auto) operator()(this Self const_& self, Arg&& arg) { \
        return ((*self_cast<V>(&self)) != std::forward<Arg>(arg)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_less { \
    template <typename Self, typename Arg> \
    decltype(auto) operator()(this Self const_& self, Arg&& arg) { \
        return ((*self_cast<V>(&self)) < std::forward<Arg>(arg)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_less_equals { \
    template <typename Self, typename Arg> \
    decltype(auto) operator()(this Self const_& self, Arg&& arg) { \
        return ((*self_cast<V>(&self)) <= std::forward<Arg>(arg)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_greater { \
    template <typename Self, typename Arg> \
    decltype(auto) operator()(this Self const_& self, Arg&& arg) { \
        return ((*self_cast<V>(&self)) > std::forward<Arg>(arg)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_greater_equals { \
    template <typename Self, typename Arg> \
   decltype(auto) operator()(this Self const_& self, Arg&& arg) { \
        return ((*self_cast<V>(&self)) >= std::forward<Arg>(arg)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_plus_equals { \
    template <typename Self, typename Arg> \
    decltype(auto) operator()(this Self const_& self, Arg&& arg) { \
        return ((*self_cast<V>(&self)) += std::forward<Arg>(arg)); \
    } \
}; \
template <typename V, std::meta::info Spec> \
struct constness##trait_invoke_op_minus_equals { \
    template <typename Self, typename Arg> \
    decltype(auto) operator()(this Self const_& self, Arg&& arg) { \
        return ((*self_cast<V>(&self)) -= std::forward<Arg>(arg)); \
    } \
}; \

__DEFINE_TRAIT_INVOKE_OP(const_, const)
__DEFINE_TRAIT_INVOKE_OP(mutable_, )

#undef __DEFINE_TRAIT_INVOKE_OP

template <typename V>
consteval std::meta::info choose_named_call(interface_spec_with_target const& spec){
    if(!is_defaulted_function_spec(spec.target)) {
        if(is_const_function(spec.target)) {
            return ^^const_trait_model_map_call;
        } else {
            return ^^mutable_trait_model_map_call;
        }
    } else if(is_class_type(^^std::remove_cvref_t<V>)){
        if(is_const_function(spec.member)) {
            return ^^const_trait_member_call;
        } else {
            return ^^mutable_trait_member_call;
        }
    }
    return {};
}

//template <std::meta::info member_call typename V, template <typename, typename, typename...> typename Trait, typename... Args>


consteval std::meta::info choose_operator_call(interface_spec_with_target const& spec){
#define __RETURN_OP(op_name) \
	if (meta::is_op_spec(spec.member, std::meta::op_##op_name)) { \
        if (is_const_function(spec.member)) { \
            return ^^const_trait_invoke_op_##op_name; \
        } else { \
            return ^^mutable_trait_invoke_op_##op_name; \
        } \
    }
    __RETURN_OP(plus_plus)
    else __RETURN_OP(minus_minus)
    else __RETURN_OP(star)
    else __RETURN_OP(arrow)
    else __RETURN_OP(parentheses)
    else __RETURN_OP(square_brackets)
    else __RETURN_OP(plus)
    else __RETURN_OP(minus)
    else __RETURN_OP(equals_equals)
    else __RETURN_OP(exclamation_equals)
    else __RETURN_OP(less)
    else __RETURN_OP(less_equals)
    else __RETURN_OP(greater)
    else __RETURN_OP(greater_equals)
    else __RETURN_OP(plus_equals)
    else __RETURN_OP(minus_equals)

#undef __RETURN_OP
}

template <typename V, template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_static_facade_call(interface_spec_with_target const& spec){

    auto self_t = is_const_function(spec.target) ? ^^const V : ^^V;

    if(auto named_call = choose_named_call<V>(spec); named_call != std::meta::info{}){
        auto args = { self_t, reflect_constant(spec.target) };
        return substitute(named_call, args);
    } else if (auto op_call = choose_operator_call(spec); op_call != std::meta::info{}) {
        auto args = { self_t, reflect_constant(spec.member) };
        return substitute(op_call, args);
    } else {
        throw std::meta::exception{std::string{display_string_of(spec.member)} + " not yet implemeted in anyxx " + std::string{display_string_of(spec.member)}, spec.member};
    }
}

}  // namespace anyxx26
