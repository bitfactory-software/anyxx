#pragma once

#include <meta>

namespace anyxx26::meta {

template <std::meta::info ...Ms>
struct outer {
    struct inner;
    consteval {
        define_aggregate(^^inner, { Ms... });
    }
};
template <std::meta::info ...Ms>
using to_struct = outer<Ms...>::inner;

template <std::meta::info Struct, std::meta::info Other>
consteval std::meta::info get_member() {
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(Struct, ctx))) {
        if constexpr(has_identifier(m) && identifier_of(m) == identifier_of(Other)){
            return m;
        }
    }
    return {};
}

template <std::meta::info Struct, std::meta::info Default>
consteval std::meta::info get_first_public_base() {
    constexpr auto ctx = std::meta::access_context::unprivileged();
    template for(constexpr auto base : define_static_array(bases_of(Struct, ctx))) {
            return base;
    }
    return Default;
}

}