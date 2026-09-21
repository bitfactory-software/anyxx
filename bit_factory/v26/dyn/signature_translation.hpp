#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/dyn/keywords.hpp>
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

consteval std::string decorated_name_of(std::meta::info in) {
    std::string name{ meta::function_name_of(in) };
    //if(is_function(in) || is_operator_function(in)) {
    //    for(auto p : parameters_of(in) | std::views::drop(is_static_member(in) ? 1 : 0)) {
    //        name += display_string_of(type_of(p));
    //    }
    //}
    //const auto r = std::ranges::remove_if(name, [](char c) { return !(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') && !(c >= '0' && c <= '9') && c != '_'; });
    //name.erase(r.begin(), r.end());
    return name;
}
consteval std::meta::info get_member_by_decorated_name(std::meta::info in, std::string_view id) {
    constexpr auto ctx = std::meta::access_context::current();
    for(auto m : members_of(in, ctx)) {
        if(decorated_name_of(m) == id) {
            return m;
        }
    }
    return {};
}

consteval bool same_signature(std::meta::info self_t, auto const& candidate_params, auto const& declaration_params) {
    if (candidate_params.size() != declaration_params.size()) {
        return false;
    }
    for (auto i : std::views::iota(0u, candidate_params.size())) {
      auto declaration_type = type_of(declaration_params[i]);
      auto candidate_type = type_of(candidate_params[i]);
      if (candidate_type == declaration_type) {
        continue;
      } else if ((declaration_type == ^^declaration const&) && (candidate_type == add_lvalue_reference(add_const(self_t)))) {
        continue;
      } else if ((declaration_type == ^^declaration&) && (candidate_type == add_lvalue_reference(self_t))) {
         continue;
      } else {
        return false;
      }
    }
    return true;
}

consteval std::meta::info get_implementation_member(std::meta::info in, std::meta::info self_t, std::meta::info declaration_member) {
    constexpr auto ctx = std::meta::access_context::current();
    for(auto candidate : members_of(in, ctx)) {
      if (meta::function_name_of(candidate) == meta::function_name_of(declaration_member)) {
        auto m_params = parameters_of(candidate) 
            | std::views::drop(is_static_member(candidate) ? 1 : 0); 
        auto d_params = parameters_of(declaration_member) 
            | std::views::drop(is_static_member(declaration_member) ? 1 : 0);
        if (same_signature(self_t, m_params, d_params)) {
            return candidate;
        }
      }
    }
    return {};
}

}  // namespace anyxx26::meta