#pragma once

#include <array>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/trait_translation/spec.hpp>
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

#define __DEFINE_TRAIT_INVOKE_OP(constness, const_) \
  template <typename V> \
struct constness##trait_invoke_op_parentheses { \
    template <typename... Args> \
    decltype(auto) operator()(Args&&... args) const_ { \
        return (*self_cast<V>(this))(std::forward<Args>(args)...); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_plus_plus { \
    template <typename... Args> \
    decltype(auto) operator()(Args&&... args) const_ { \
        return ++(*self_cast<V>(this)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_minus_minus { \
    template <typename... Args> \
    decltype(auto) operator()(Args&&... args) const_ { \
        return --(*self_cast<V>(this)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_star { \
    template <typename... Args> \
    decltype(auto) operator()(Args&&... args) const_ { \
        if constexpr(sizeof...(Args) == 0) { \
            return *(*self_cast<V>(this)); \
        } else { \
            return ((*self_cast<V>(this)) * ... * std::forward<Args>(args)); \
        } \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_arrow { \
    template <typename... Args> \
    decltype(auto) operator()(Args&&... args) const_ { \
        return std::to_address(*self_cast<V>(this)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_square_brackets { \
    template <typename... Args> \
    decltype(auto) operator()(Args&&... args) const_ { \
        return (*self_cast<V>(this))[std::forward<Args>(args)...]; \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_plus { \
    template <typename... Args> \
    decltype(auto) operator()(Args&&... args) const_ { \
        if constexpr(sizeof...(Args) == 0) { \
            return +(*self_cast<V>(this)); \
        } else { \
            return ((*self_cast<V>(this)) + ... + std::forward<Args>(args)); \
        } \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_minus { \
    template <typename... Args> \
    decltype(auto) operator()(Args&&... args) const_ { \
        if constexpr(sizeof...(Args) == 0) { \
            return -(*self_cast<V>(this)); \
        } else { \
            return ((*self_cast<V>(this)) - ... - std::forward<Args>(args)); \
        } \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_equals_equals { \
    template <typename Arg> \
    decltype(auto) operator()(Arg&& arg) const_ { \
        return ((*self_cast<V>(this)) == std::forward<Arg>(arg)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_exclamation_equals { \
    template <typename Arg> \
    decltype(auto) operator()(Arg&& arg) const_ { \
        return ((*self_cast<V>(this)) != std::forward<Arg>(arg)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_less { \
    template <typename Arg> \
    decltype(auto) operator()(Arg&& arg) const_ { \
        return ((*self_cast<V>(this)) < std::forward<Arg>(arg)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_less_equals { \
    template <typename Arg> \
    decltype(auto) operator()(Arg&& arg) const_ { \
        return ((*self_cast<V>(this)) <= std::forward<Arg>(arg)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_greater { \
    template <typename Arg> \
    decltype(auto) operator()(Arg&& arg) const_ { \
        return ((*self_cast<V>(this)) > std::forward<Arg>(arg)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_greater_equals { \
    template <typename Arg> \
   decltype(auto) operator()(Arg&& arg) const_ { \
        return ((*self_cast<V>(this)) >= std::forward<Arg>(arg)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_plus_equals { \
    template <typename Arg> \
    decltype(auto) operator()(Arg&& arg) const_ { \
        return ((*self_cast<V>(this)) += std::forward<Arg>(arg)); \
    } \
}; \
template <typename V> \
struct constness##trait_invoke_op_minus_equals { \
    template <typename Arg> \
    decltype(auto) operator()(Arg&& arg) const_ { \
        return ((*self_cast<V>(this)) -= std::forward<Arg>(arg)); \
    } \
}; \

__DEFINE_TRAIT_INVOKE_OP(const, const)
__DEFINE_TRAIT_INVOKE_OP(mutable, )

#undef __DEFINE_TRAIT_INVOKE_OP

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

}  // namespace anyxx26
