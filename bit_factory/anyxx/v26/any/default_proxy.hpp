#pragma once

#include <bit_factory/anyxx/v26/any/keywords.hpp>
#include <bit_factory/anyxx/observeable.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <meta>

namespace anyxx {

template <template <typename, typename...> typename Trait, typename... Args>
concept specifies_default_proxy_t = requires { typename trait_declaration_t<Trait, Args...>::default_proxy_t; };

template <template <typename, typename...> typename Trait, typename... Args>
consteval std::meta::info compute_default_proxy_t() {
    if constexpr(specifies_default_proxy_t<Trait, Args...>) {
        return ^^typename trait_declaration_t<Trait, Args...>::default_proxy_t;
    } else {
        return ^^anyxx::cref;
    }
}

template <template <typename, typename...> typename Trait, typename... Args>
using default_proxy_t = [:compute_default_proxy_t<Trait, Args...>():];

}