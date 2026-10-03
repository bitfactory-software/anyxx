#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <meta>

namespace anyxx26 {

struct declaration {};
struct model_map {};
template <typename Trait>
concept is_trait = std::same_as<Trait, declaration> || std::same_as<Trait, model_map>;

template <template <is_trait, typename, typename...> typename Trait>
using base_v_table_t = anyxx::observeable::v_table_t;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
struct v_table;

template <template <is_trait, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct any_base;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
struct any;

template <typename V, template <is_trait, typename, typename...> typename Trait, typename... Args>
class trait_as;

struct default_t {};
constexpr static inline default_t defaulted = {};

template <typename V, typename Self>
constexpr decltype(auto) self_cast(Self* self){
    return static_cast<V*>(static_cast<std::conditional_t<std::is_const_v<Self>, const void, void>*>(self));
}

struct v_table_data_t {};
constexpr static inline v_table_data_t v_table_data = {};

template <typename V, typename VoidSelf>
using self_const_correct_t = std::conditional_t<
    std::is_const_v<std::remove_pointer_t<std::remove_reference_t<VoidSelf>>>,
    V const, V>;
consteval std::meta::info void_self(bool is_const) {
  if (is_const) {
    return ^^void const*;
  } else {
    return ^^void*;
  }
}

consteval std::meta::info trait_for(std::meta::info trait_template, std::meta::info type, std::meta::info mapped_type, auto args){
    std::vector<std::meta::info> params{ type, mapped_type };
    params.append_range(args);
    return substitute(trait_template, params);
}
consteval std::meta::info trait_model_map(std::meta::info trait_template, std::meta::info mapped_type, auto args){
  return trait_for(trait_template, ^^model_map, mapped_type, args);
}
template<std::meta::info TraitTemplate, typename V, std::meta::info... Args>
consteval std::meta::info trait_model_map(){
    return trait_model_map(TraitTemplate, ^^V, std::vector<std::meta::info>{ Args... });
}

consteval std::meta::info trait_declaration(std::meta::info trait_template, auto args){
    return trait_for(trait_template, ^^declaration, ^^declaration, args);
}
template<std::meta::info TraitTemplate, std::meta::info... Args>
consteval std::meta::info trait_declaration(){
    return trait_declaration(TraitTemplate, std::vector<std::meta::info>{ Args... });
}
template<template<typename, typename...> typename TraitTemplate, typename... Args>
using trait_declaration_t = TraitTemplate<declaration, declaration, Args...>;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
concept specifies_default_proxy_t = requires { typename trait_declaration_t<Trait, Args...>::default_proxy_t; };

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info compute_default_proxy_t() {
    if constexpr(specifies_default_proxy_t<Trait, Args...>) {
        return ^^typename trait_declaration_t<Trait, Args...>::default_proxy_t;
    } else {
        return ^^anyxx::cref;
    }
}

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using default_proxy_t = [:compute_default_proxy_t<Trait, Args...>():];

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_val_t = any<Trait, Args...>;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_cref_t = any<Trait, anyxx::cref, Args...>;

template <template <is_trait, typename, typename...> typename Trait, typename... Args>
using dyn_self_mutref_t = any<Trait, anyxx::mutref, Args...>;

struct empty_t {};

template <template <typename, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct any_base;

template <typename HasTraitDeclaration>
concept has_trait_declaration =
    requires {
      typename HasTraitDeclaration::trait_declaration_t;
    };

}  // namespace anyxx26

namespace anyxx {
template <anyxx26::has_trait_declaration Derived,
          anyxx26::has_trait_declaration Base>
constexpr bool is_any_derived_from_v<Derived, Base> =
    std::derived_from<typename Derived::trait_declaration_t, typename Base::trait_declaration_t>;
}
