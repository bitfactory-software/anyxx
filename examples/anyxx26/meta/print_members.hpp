#pragma once

#include <meta>
#include <print>

namespace anyxx26::meta {

template <typename T>
void print_members() {
    constexpr auto ctx = std::meta::access_context::current();
    constexpr auto type_info = ^^T;
    constexpr static auto members = std::define_static_array(members_of(type_info, ctx));
    template for(constexpr std::meta::info m : members) {
        if constexpr(has_identifier(m) && is_static_member(m) &&
            is_function(m)) {
            std::println("{}", std::meta::display_string_of(m));
        }
    }
}

}  // namespace anyxx26::meta