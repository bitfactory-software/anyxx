#pragma once

#include <examples/anyxx26/meta/utilities.hpp>

#include <meta>
#include <utility>
#include <vector>
#include <array>



namespace anyxx26 {

struct default_t{};
constexpr static inline default_t defaulted = {};

template <std::meta::info m, typename V, typename R, typename... Args>
R default_impl(void* self, Args&&... args){
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto mc : define_static_array(members_of(^^ V, ctx))) {
        if constexpr(has_identifier(mc) && !is_static_member(mc) && is_function(mc) &&
            identifier_of(mc) == identifier_of(m)) {
            return static_cast<V*>(self)->[:mc:](std::forward<Args>(args)...);
        }
    }
}

template <typename R, typename... Args>
using v_table_fptr_type = R(*)(void*, Args...);

consteval std::meta::info make_v_table_fptr_type(const std::meta::info f){
    std::vector<std::meta::info> types;
    types.push_back(return_type_of(f));
    auto i = 0;
    for(auto p : parameters_of(f)) {
        ++i;
        if(i > 1) {
            auto type = type_of(p);
            types.push_back(type);
        }
    }
    return substitute(^^v_table_fptr_type, types);
}

template<template <typename> typename Trait>
consteval std::meta::info make_v_table_type(){
    std::vector<std::meta::info> fptrs;
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(^^ Trait<void*>, ctx))) {
        if constexpr(has_identifier(m) && is_static_member(m) && is_function(m)){
            auto ft = make_v_table_fptr_type(m);
            auto dms = std::meta::data_member_spec(ft, { .name = identifier_of(m), .no_unique_address = true });
            fptrs.push_back(reflect_constant(dms));
        }
    }
    return substitute(^^meta::to_struct, fptrs);
};

template <typename VTable, auto... Functions>
inline auto v_table_instance = VTable{ Functions... };

template <bool default_, std::meta::info m, typename V, typename R, typename... Args>
R vfimpl(void* self, Args&&... args){
    if constexpr(default_) {
        return default_impl<m, V, R>(self, std::forward<Args>(args)...);
    } else {
        return[:m:](*static_cast<V*>(self), std::forward<Args>(args)...);
    }
}

template <typename V>
consteval std::meta::info make_vfimpl(std::meta::info f){
    std::vector<std::meta::info> types;
    bool use_default = annotations_of_with_type(f, ^^ default_t).size() > 0;
    types.push_back(std::meta::reflect_constant(use_default));
    types.push_back(reflect_constant(f));
    types.push_back(^^V);
    types.push_back(return_type_of(f));
    auto i = 0;
    for(auto p : parameters_of(f)) {
        ++i;
        if(i > 1) {
            auto type = type_of(p);
            types.push_back(type);
        }
    }
    return substitute(^^vfimpl, types);
}

template<template <typename> typename Trait, typename V>
consteval std::meta::info make_v_table_instance(){
    std::vector<std::meta::info> types;
    types.push_back(make_v_table_type<Trait>());
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(^^ Trait<V>, ctx))) {
        if constexpr(has_identifier(m) && is_static_member(m) && is_function(m)){
            types.push_back(make_vfimpl<V>(m));
        }
    }
    return substitute(^^v_table_instance, types);
};

template<template <typename> typename Trait>
struct dyn_base {
    [:make_v_table_type<Trait>() :] const& v_table;
    void* self = nullptr;
    template <typename V>
        requires (!std::derived_from<V, dyn_base>)
    dyn_base(V& v) : v_table([:make_v_table_instance<Trait, V>() :]), self(&v) {}
};

template<template <typename> typename Trait, std::meta::info vf> struct dyn_facade_call {
    template <typename... Args>
    auto operator()(Args&&... args) const {
        auto base = reinterpret_cast<dyn_base<Trait>const*>(this);
        auto v_table = base->v_table;
        auto self = base->self;
        return v_table.[:vf:](self, std::forward<Args>(args)...);
    }
};

template<template <typename> typename Trait>
consteval std::meta::info make_dyn_facade(){
    constexpr auto v_table_t_info = make_v_table_type<Trait>();

    std::vector<std::meta::info> calls;
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(v_table_t_info, ctx))) {
        if constexpr(has_identifier(m)){
            using dyn_facade_call_t = dyn_facade_call<Trait, m>;
            constexpr std::meta::info call_meta = ^^dyn_facade_call_t;
            auto dms = std::meta::data_member_spec(call_meta, { .name = identifier_of(m), .no_unique_address = true });
            calls.push_back(reflect_constant(dms));
        }
    }
    return substitute(^^meta::to_struct, calls);
};

template<template <typename> typename Trait> struct dyn :
    dyn_base<Trait>,
    [:make_dyn_facade<Trait>() :] {
    using dyn_base<Trait>::dyn_base;
};

} // namespace anyxx26

