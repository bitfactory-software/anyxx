#pragma once

#include <bit_factory/anyxx/v26/any/keywords.hpp>
#include <bit_factory/anyxx/v26/any/default_proxy.hpp>
#include <bit_factory/anyxx/proxies/dyn/observer.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/any_concept.hpp>

namespace anyxx {

template <typename Proxy, template <is_trait, typename, typename...> typename Trait, typename... Args>
struct any_;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
struct any;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_val_t = any<Trait, Args...>;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_cref_t = any_<cref, Trait, Args...>;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_mutref_t = any_<mutref, Trait, Args...>;

template <typename HasTraitDeclaration>
concept has_trait_declaration =
    requires {
    typename HasTraitDeclaration::trait_declaration_t;
};

template <typename Derived, typename Base>
struct is_any_derived_from_v {
    static constexpr bool value = std::derived_from<typename Derived::trait_declaration_t, typename Base::trait_declaration_t>;
};

}  // namespace anyxx