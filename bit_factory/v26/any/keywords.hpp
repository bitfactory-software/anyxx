#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <meta>

namespace anyxx26 {

struct declaration {};
struct model_map {};
template <typename Trait>
concept is_trait = std::same_as<Trait, declaration> || std::same_as<Trait, model_map>;

template <template <typename, typename, typename...> typename Trait>
using base_v_table_t = anyxx::observeable::v_table_t;

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct v_table;

template <template <typename, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct any_base;

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct any;

struct default_t {};
constexpr static inline default_t defaulted = {};



struct v_table_data_t {};
constexpr static inline v_table_data_t v_table_data = {};

template <typename V, typename VoidSelf>
using self_const_correct_t = std::conditional_t<
    std::is_const_v<std::remove_pointer_t<std::remove_reference_t<VoidSelf>>>,
    V const, V>;

consteval std::meta::info trait_model_map(std::meta::info trait_template, std::meta::info mapped_type, auto args){
  std::vector<std::meta::info> params{ ^^model_map, mapped_type };
  params.append_range(args);
  return substitute(trait_template, params);
}
template<std::meta::info TraitTemplate, typename V, std::meta::info... Args>
consteval std::meta::info trait_model_map(){
    return trait_model_map(TraitTemplate, ^^V, std::vector<std::meta::info>{ Args... });
}

consteval std::meta::info trait_declaration(std::meta::info trait_template, auto args){
  std::vector<std::meta::info> params{ ^^declaration, ^^declaration };
  params.append_range(args);
  return substitute(trait_template, params);
}
template<std::meta::info TraitTemplate, std::meta::info... Args>
consteval std::meta::info trait_declaration(){
    return trait_declaration(TraitTemplate, std::vector<std::meta::info>{ Args... });
}
template<template<typename, typename...> typename TraitTemplate, typename... Args>
using trait_declaration_t = TraitTemplate<declaration, declaration, Args...>;

template <template <typename, typename, typename...> typename Trait, typename... Args>
concept specifies_default_proxy_t = requires { typename trait_declaration_t<Trait, Args...>::default_proxy_t; };

template <template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info compute_default_proxy_t() {
    if constexpr(specifies_default_proxy_t<Trait, Args...>) {
        return ^^typename trait_declaration_t<Trait, Args...>::default_proxy_t;
    } else {
        return ^^anyxx::cref;
    }
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
using default_proxy_t = [:compute_default_proxy_t<Trait, Args...>():];

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_val_t = any<Trait, Args...>;

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_cref_t = any<Trait, anyxx::cref, Args...>;

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_mutref_t = any<Trait, anyxx::mutref, Args...>;

}  // namespace anyxx26
