#pragma once

#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
#include <bit_factory/v26/trait_as/signature_translation.hpp>

namespace anyxx26 {

template <typename V, std::meta::info Target, std::meta::info Spec, typename TraitAs, typename... Args>
struct const_trait_model_map_call {
  template <typename Self>
  constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const& self, Args... args){
      constexpr auto return_type = return_type_of(Spec);
      if constexpr(return_type == ^^void){
          [:Target:](*self_cast<V>(&self), forward_trait_as_param<TraitAs, Spec, Args>(args)...);
          return;
      } else {
          return forward_trait_as_return<TraitAs, Spec>(self, [:Target:](*self_cast<V>(&self), forward_trait_as_param<TraitAs, Spec, Args>(args)...));
      }
  }
};
template <typename V, std::meta::info Target, std::meta::info Spec, typename TraitAs, typename... Args>
struct mutable_trait_model_map_call {
    template <typename Self>
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self&  self, Args... args) {
        constexpr auto return_type = return_type_of(Spec);
        if constexpr(return_type == ^^void){
            [:Target:] (*self_cast<V>(&self), forward_trait_as_param<TraitAs, Spec, Args>(args)...);
            return;
        } else {
            return forward_trait_as_return<TraitAs, Spec>(self, [:Target:](*self_cast<V>(&self), forward_trait_as_param<TraitAs, Spec, Args>(args)...));
        }
    }
};

template <typename V, std::meta::info Target, std::meta::info Spec, typename TraitAs, typename... Args>
struct const_trait_member_call {
    template <typename Self>
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const& self, Args... args) {
        constexpr auto return_type = return_type_of(Spec);
        if constexpr (return_type == ^^void) {
            self_cast<V>(&self)->[:Target:](forward_trait_as_param<TraitAs, Spec, Args>(args)...);
            return;
        } else {
            return forward_trait_as_return<TraitAs, Spec>(self, self_cast<V>(&self)->[:Target:](forward_trait_as_param<TraitAs, Spec, Args>(args)...));
        }
    }
};
template <typename V, std::meta::info Target, std::meta::info Spec, typename TraitAs, typename... Args>
struct mutable_trait_member_call {
    template <typename Self>
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self&  self, Args... args) {
        constexpr auto return_type = return_type_of(Spec);
        if constexpr (return_type == ^^void) {
            self_cast<V>(&self)->[:Target:](forward_trait_as_param<TraitAs, Spec, Args>(args)...);
            return;
        } else {
            return forward_trait_as_return<TraitAs, Spec>(self, self_cast<V>(&self)->[:Target:](forward_trait_as_param<TraitAs, Spec, Args>(args)...));
        }
    }
};

#define __DEFINE_TRAIT_INVOKE_OP(constness, const_) \
template <typename V, std::meta::info Spec, typename TraitAs, typename... Args> \
struct constness##trait_invoke_op_parentheses { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Args... args) { \
        return forward_trait_as_return<TraitAs, Spec>(self, (*self_cast<V>(&self))(forward_trait_as_param<TraitAs, Spec, Args>(args)...)); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename... Args> \
struct constness##trait_invoke_op_plus_plus { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Args... args) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ++(*self_cast<V>(&self))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename... Args> \
struct constness##trait_invoke_op_minus_minus { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Args... args) { \
        return forward_trait_as_return<TraitAs, Spec>(self, --(*self_cast<V>(&self))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename... Args> \
struct constness##trait_invoke_op_star { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Args... args) { \
        if constexpr(sizeof...(Args) == 0) { \
            return forward_trait_as_return<TraitAs, Spec>(self, *(*self_cast<V>(&self))); \
        } else { \
            return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) * ... * forward_trait_as_param<TraitAs, Spec, Args>(args))); \
        } \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename... Args> \
struct constness##trait_invoke_op_arrow { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Args... args) { \
        return forward_trait_as_return<TraitAs, Spec>(self, std::to_address(*self_cast<V>(&self))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename... Args> \
struct constness##trait_invoke_op_square_brackets { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Args... args) { \
        return forward_trait_as_return<TraitAs, Spec>(self, (*self_cast<V>(&self))[forward_trait_as_param<TraitAs, Spec, Args>(args)...]); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename... Args> \
struct constness##trait_invoke_op_plus { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Args... args) { \
        if constexpr(sizeof...(Args) == 0) { \
            return forward_trait_as_return<TraitAs, Spec>(self, +(*self_cast<V>(&self))); \
        } else { \
            return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) + ... + forward_trait_as_param<TraitAs, Spec, Args>(args))); \
        } \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename... Args> \
struct constness##trait_invoke_op_minus { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Args... args) { \
        if constexpr(sizeof...(Args) == 0) { \
            return forward_trait_as_return<TraitAs, Spec>(self, -(*self_cast<V>(&self))); \
        } else { \
            return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) - ... - forward_trait_as_param<TraitAs, Spec, Args>(args))); \
        } \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename Arg> \
struct constness##trait_invoke_op_equals_equals { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Arg arg) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) == forward_trait_as_param<TraitAs, Spec, Arg>(arg))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename Arg> \
struct constness##trait_invoke_op_exclamation_equals { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Arg arg) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) != forward_trait_as_param<TraitAs, Spec, Arg>(arg))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename Arg> \
struct constness##trait_invoke_op_less { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Arg arg) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) < forward_trait_as_param<TraitAs, Spec, Arg>(arg))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename Arg> \
struct constness##trait_invoke_op_less_equals { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Arg arg) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) <= forward_trait_as_param<TraitAs, Spec, Arg>(arg))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename Arg> \
struct constness##trait_invoke_op_greater { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Arg arg) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) > forward_trait_as_param<TraitAs, Spec, Arg>(arg))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename Arg> \
struct constness##trait_invoke_op_greater_equals { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Arg arg) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) >= forward_trait_as_param<TraitAs, Spec, Arg>(arg))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename Arg> \
struct constness##trait_invoke_op_plus_equals { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Arg arg) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) += forward_trait_as_param<TraitAs, Spec, Arg>(arg))); \
    } \
}; \
template <typename V, std::meta::info Spec, typename TraitAs, typename Arg> \
struct constness##trait_invoke_op_minus_equals { \
    template <typename Self> \
    constexpr trait_as_return_type_t<TraitAs, Spec> operator()(this Self const_& self, Arg arg) { \
        return forward_trait_as_return<TraitAs, Spec>(self, ((*self_cast<V>(&self)) -= forward_trait_as_param<TraitAs, Spec, Arg>(arg))); \
    } \
}; \

__DEFINE_TRAIT_INVOKE_OP(const_, const)
__DEFINE_TRAIT_INVOKE_OP(mutable_, )

#undef __DEFINE_TRAIT_INVOKE_OP

template <typename V>
consteval std::meta::info choose_named_call(interface_spec_with_target const& spec){
    if(spec.target != std::meta::info{} && !is_defaulted_function_spec(spec.target)) {
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

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval auto make_trait_as_named_call_template_params(interface_spec_with_target const& spec) {
    auto self_t = is_const_function(spec.member) ? ^^ const V : ^^V;
    using trait_as_t = trait_as<V, Trait, Args...>;
    std::vector<std::meta::info> args = { self_t, reflect_constant(spec.target), reflect_constant(spec.member), ^^trait_as_t };
    args.append_range(make_trait_as_params<trait_as_t>(spec.member));
    return args;
}

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval auto make_trait_as_op_call_template_params(interface_spec_with_target const& spec) {
    auto self_t = is_const_function(spec.member) ? ^^ const V : ^^V;
    using trait_as_t = trait_as<V, Trait, Args...>;
    std::vector<std::meta::info> args = { self_t, reflect_constant(spec.member), is_const_function(spec.member) ? ^^const trait_as_t : ^^trait_as_t };
    args.append_range(make_trait_as_params<trait_as_t>(spec.member));
    return args;
}

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_static_facade_call(interface_spec_with_target const& spec){
    if(auto named_call = choose_named_call<V>(spec); named_call != std::meta::info{}){
        return substitute(named_call, make_trait_as_named_call_template_params<V, Trait, Args...>(spec));
    } else if (auto op_call = choose_operator_call(spec); op_call != std::meta::info{}) {
        return substitute(op_call, make_trait_as_op_call_template_params<V, Trait, Args...>(spec));
    } else {
        throw std::meta::exception{std::string{display_string_of(spec.member)} + " not yet implemeted in anyxx " + std::string{display_string_of(spec.member)}, spec.member};
    }
}

}  // namespace anyxx26
