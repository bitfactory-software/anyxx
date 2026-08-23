#pragma once

#include <examples/anyxx26/meta/utilities.hpp>

#include <meta>
#include <utility>
#include <vector>
#include <array>

namespace anyxx26 {

template<typename V, std::meta::info Target>
struct trait_facade_call {
    template <typename... Args>
    auto operator()(Args&&... args) const {
        const V* pvalue = reinterpret_cast<const V*>(this);
        return[:Target:](*pvalue, std::forward<Args>(args)...);
    }
};

template<template <typename> typename Trait, typename V>
consteval std::meta::info make_trait_facade(){
    std::vector<std::meta::info> calls;
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(^^ Trait<V>, ctx))) {
        if constexpr(has_identifier(m) && is_static_member(m) && is_function(m)){
            using trait_facade_call_t = trait_facade_call<V, m>;
            constexpr std::meta::info wrapped_meta = ^^trait_facade_call_t;
            auto dms = std::meta::data_member_spec(wrapped_meta, { .name = identifier_of(m), .no_unique_address = true });
            calls.push_back(reflect_constant(dms));
        }
    }
    return substitute(^^meta::to_struct, calls);
};

template<typename V, template<typename> typename Trait>
class trait_as : public [:make_trait_facade<Trait, V>() :] {
    V value_;
public:
    trait_as(V const& value) : value_(value) {}
};

}
