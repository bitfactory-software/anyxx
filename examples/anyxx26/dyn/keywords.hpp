#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <meta>

namespace anyxx26 {

template <template <typename, typename, typename...> typename Trait>
using base_v_table_t = anyxx::observeable::v_table_t;

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct v_table;

template <template <typename, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct dyn_base;

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct dyn;

struct default_t {};
constexpr static inline default_t defaulted = {};

struct declaration {};
struct model_map {};

struct v_table_data_t {};
constexpr static inline v_table_data_t v_table_data = {};

template <typename V, typename VoidSelf>
using self_const_correct_t = std::conditional_t<
    std::is_const_v<std::remove_pointer_t<std::remove_reference_t<VoidSelf>>>,
    V const, V>;

consteval std::meta::info trait_model_map(std::meta::info trait_template, std::meta::info mapped_type, std::vector<std::meta::info> const& args){
  std::vector<std::meta::info> params{ mapped_type, ^^ model_map };
  params.append_range(args);
  return substitute(trait_template, params);
}
template<std::meta::info TraitTemplate, typename V, std::meta::info... Args>
consteval std::meta::info trait_model_map(){
    return trait_model_map(TraitTemplate, ^^V, { Args... });
}

consteval std::meta::info trait_declaration(std::meta::info trait_template, std::vector<std::meta::info> const& args){
  std::vector<std::meta::info> params{ ^^declaration, ^^declaration };
  params.append_range(args);
  return substitute(trait_template, params);
}
template<std::meta::info TraitTemplate, std::meta::info... Args>
consteval std::meta::info trait_declaration(){
    return trait_declaration(TraitTemplate, { Args... });
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
using dyn_self_val_t = dyn<Trait, Args...>;

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_cref_t = dyn<Trait, anyxx::cref, Args...>;

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_mutref_t = dyn<Trait, anyxx::mutref, Args...>;

}  // namespace anyxx26
