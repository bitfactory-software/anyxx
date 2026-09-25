#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>

namespace anyxx26 {

template <>
consteval std::meta::info find_candidate_in_target(std::meta::info spec, std::meta::info target, std::meta::info return_type, std::meta::info void_self, std::meta::info... args) {
    constexpr auto ctx = std::meta::access_context::current();
    using self_t = self_const_correct_t<target, void_self>;
    template for(constexpr auto candidate : define_static_array(members_of(target, ctx))) {
        if constexpr(!is_static_member(candidate) && is_function(candidate)) {
            if constexpr(meta::function_name_of(candidate) == meta::function_name_of(spec)) {
                if constexpr(std::is_invocable_r_v<[:return_type:], decltype(&[:candidate:]), self_t, [:args:]...>) {
                    return candidate;
                }
            }
        }
    }
    return {};
}

}  // namespace anyxx26::meta