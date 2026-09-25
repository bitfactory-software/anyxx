#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>

namespace anyxx26 {

template <std::meta::info spec, typename Target, typename R, typename VoidSelf, typename... Args>
consteval std::meta::info find_candidate_in_target() {
    constexpr auto ctx = std::meta::access_context::current();
    using self_t = self_const_correct_t<Target, VoidSelf>;
    template for(constexpr auto candidate : define_static_array(members_of(^^Target, ctx))) {
        if constexpr(!is_static_member(candidate) && is_function(candidate)) {
            if constexpr(meta::function_name_of(candidate) == meta::function_name_of(spec)) {
                if constexpr(std::is_invocable_r_v<R, decltype(&[:candidate:]), self_t, Args...>) {
                    return candidate;
                }
            }
        }
    }
    return {};
}

consteval std::vector<std::meta::info> make_find_candidate_in_target_params(std::meta::info interface_function, std::meta::info concrete_type){
    std::vector<std::meta::info> types;
    types.push_back(reflect_constant(interface_function));
    types.push_back(concrete_type);
    types.push_back(return_type_of(interface_function));
    types.push_back(void_self(is_const_function(interface_function)));
    for(auto p : define_static_array(parameters_of(interface_function) | std::views::drop(is_static_member(interface_function) ? 1 : 0))) {
        types.push_back(type_of(p));
    }
    return types;
}

template <typename V, std::meta::info interface_function>
consteval std::meta::info find_candidate_in() {
    constexpr auto find_candidate_in_target_f = substitute(^^find_candidate_in_target, make_find_candidate_in_target_params(interface_function, ^^ V));
    return[:find_candidate_in_target_f:]();
}

}  // namespace anyxx26