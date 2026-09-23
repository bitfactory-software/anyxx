#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <bit_factory/v26/trait_translation/is_const_function.hpp>

namespace anyxx26 {

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
        if (is_const_function(candidate) != is_const_function(declaration_member)) {
          continue;
        }
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

consteval std::meta::info find_function_impl(std::meta::info interface_function, std::meta::info trait_template, std::meta::info mapped_type, auto args) {
    if(auto found_in_impl = get_implementation_member(trait_model_map(trait_template, mapped_type, args), mapped_type, interface_function); found_in_impl != std::meta::info{}) {
        return found_in_impl;
    } else if(auto found_in_base = get_implementation_member(trait_declaration(trait_template, args), mapped_type, interface_function); found_in_base != std::meta::info{}) {
        return found_in_base;
    } else {
        throw std::logic_error("Function not found in impl trait or base trait");
    }
}

}  // namespace anyxx26::meta