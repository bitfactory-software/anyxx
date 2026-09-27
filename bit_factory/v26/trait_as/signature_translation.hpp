#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/meta/utilities.hpp>

namespace anyxx26 {

template <typename Self>
consteval std::meta::info translate_trait_as_return_type(std::meta::info spec) {
    std::meta::info return_type = return_type_of(spec);
    if constexpr(^^return_type == ^^declaration&) {
        return ^^Self&;
    } else if constexpr(^^return_type == ^^declaration) {
        return ^^Self;
    } else {
        return return_type;
    }
}
template <typename Self, std::meta::info Spec>
using trait_as_return_type_t = [:translate_trait_as_return_type<Self>(Spec):];

template <typename Self>
consteval std::meta::info translate_trait_as_param_type(std::meta::info param){
    if(param == ^^declaration const&) {
        return ^^Self const&;
    } else if(param == ^^declaration&) {
        return ^^Self&;
    } else {
        return param;
    }
}

}  // namespace anyxx26::meta