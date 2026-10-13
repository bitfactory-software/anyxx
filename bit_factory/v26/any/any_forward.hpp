#pragma once

#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/anyxx/proxies/dyn/observer.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/any_concept.hpp>

namespace anyxx {

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
struct any;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_val_t = any<Trait, Args...>;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_cref_t = any<Trait, cref, Args...>;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_mutref_t = any<Trait, mutref, Args...>;

template <typename HasTraitDeclaration>
concept has_trait_declaration =
    requires {
    typename HasTraitDeclaration::trait_declaration_t;
};

template <has_trait_declaration Derived,
    has_trait_declaration Base>
constexpr bool is_any_derived_from_v<Derived, Base> =
std::derived_from<typename Derived::trait_declaration_t, typename Base::trait_declaration_t>;

}  // namespace anyxx