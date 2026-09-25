#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>

namespace anyxx26 {

template <std::meta::info spec, typename Target, typename R, typename VoidSelf, typename... Args>
consteval std::meta::info find_candidate_in_target() {
    constexpr auto ctx = std::meta::access_context::current();
    using self_t = self_const_correct_t<Target, VoidSelf>;
    template for(constexpr auto candidate : define_static_array(members_of(^^ Target, ctx))) {
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

}  // namespace anyxx26::meta