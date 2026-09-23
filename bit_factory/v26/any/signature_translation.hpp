#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/meta/utilities.hpp>

namespace anyxx26 {

template <typename R, typename V>
consteval std::meta::info translate_impl_return_type() {
    if constexpr(^^R == ^^declaration&) {
        return ^^V&;
    } else if constexpr(^^R == ^^declaration) {
        return ^^V;
    } else {
        return ^^R;
    }
}
template <typename R, typename V>
using impl_return_type = [:translate_impl_return_type<R, V>():];


template <std::meta::info SelfValType>
consteval std::meta::info translate_v_table_return_type(std::meta::info R) {
    if(R == ^^declaration&) {
        return ^^void;
    } else if(R == ^^declaration){
        return SelfValType;
    } else {
        return R;
    }
}
static_assert(translate_v_table_return_type<^^ int>(^^ declaration&) == ^^void);
static_assert(std::same_as<typename[:translate_v_table_return_type< ^^int>(^^ declaration&):], void>);

consteval std::meta::info translate_v_table_fptr_param_type(std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, std::meta::info param){
    if(param == ^^declaration const&) {
        return dyn_self_cref;
    } else if(param == ^^declaration&) {
        return dyn_self_mutref;
    } else {
        return param;
    }
}

template <std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, typename Param>
using translate_v_table_fptr_param_type_t = [:translate_v_table_fptr_param_type(dyn_self_cref, dyn_self_mutref, ^^ Param):];

template <typename V, typename Param>
consteval std::meta::info translate_impl_fptr_param() {
    if constexpr(^^Param == ^^declaration const&) {
        return ^^V const&;
    } else if constexpr(^^Param == ^^declaration&) {
        return ^^V&;
    } else {
        return ^^Param;
    }
}
template <typename V, typename Param>
using impl_fptr_param_t = [:translate_impl_fptr_param<V, Param>():];

template <typename V, typename Param>
decltype(auto) forward_v_table_fptr_param(auto&& param){
    if constexpr(^^Param == ^^declaration const&) {
        return std::forward<V const&>(*unerase_cast<V>(param));
    } else if constexpr(^^Param == ^^declaration&) {
        return std::forward<V&>(*unerase_cast<V>(param));
    } else {
        return std::forward<Param>(param);
    }
}

consteval std::meta::info translate_facade_return_type(std::meta::info return_type, std::meta::info dyn_self_val) {
    if(return_type == ^^declaration&) {
        return add_lvalue_reference(dyn_self_val);
    } else if(return_type == ^^declaration){
        return dyn_self_val;
    } else {
        return return_type;
    }
}

}  // namespace anyxx26::meta