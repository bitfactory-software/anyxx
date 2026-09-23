#pragma once

#include <meta>
#include <algorithm>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <bit_factory/v26/trait_interface/spec.hpp>

namespace anyxx26 {

consteval std::meta::info make_v_table_fptr_param_type(bool self, std::meta::info p) {
    auto type = type_of(p);
    if(self) {
        if(is_const(remove_reference(type))) {
            return ^^void const*;
        } else {
            return ^^void*;
        }
    } else {
        return type;
    }
}

consteval void add_v_table_fptr_this_param_type(std::meta::info f, std::vector<std::meta::info>& types) {
    if(!is_static_member(f)) {
        if(is_const(f)) {
            types.push_back(^^void const*);
        } else {
            types.push_back(^^void*);
        }
    }
}

template <std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, typename R, typename... Args>
using v_table_fptr_type = R(*)(translate_v_table_fptr_param_type_t<dyn_self_cref, dyn_self_mutref, Args>...);

template <std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref>
consteval std::meta::info make_v_table_fptr_type(std::meta::info f) {
    std::vector<std::meta::info> types;
    types.push_back(reflect_constant(dyn_self_cref));
    types.push_back(reflect_constant(dyn_self_mutref));
    types.push_back(translate_v_table_return_type<dyn_self_val>(return_type_of(f)));
    add_v_table_fptr_this_param_type(f, types);
    for(auto p : define_static_array(parameters_of(f))) {
        types.push_back(make_v_table_fptr_param_type(types.size() == 3, p));
    }
    return substitute(^^v_table_fptr_type, types);
}

template <std::meta::info TraitDeclaration, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref>
consteval std::vector<std::meta::info> collect_v_table_members() {

    std::vector<std::meta::info> fptrs;
    template for(constexpr auto spec : define_static_array(get_v_table_specs(TraitDeclaration))) {
        if constexpr(is_v_table_data(spec)) {
            using type = [:spec.member:]::type;
            auto dms = std::meta::data_member_spec(dealias(^^type), {.name = v_table_name_of(spec)});
            fptrs.push_back(reflect_constant(dms));
        }
        else {
            auto function_ptr_type = make_v_table_fptr_type<dyn_self_val, dyn_self_cref, dyn_self_mutref>(spec.member);
            auto dms = std::meta::data_member_spec(function_ptr_type, {.name = v_table_name_of(spec)});
            fptrs.push_back(reflect_constant(dms));
        }
    }
    return fptrs;
};

template <template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_v_table_members_type() {
    auto fptrs = collect_v_table_members<^^Trait<declaration, declaration, Args...>, 
        ^^dyn_self_val_t<Trait, Args...>, ^^dyn_self_cref_t<Trait, Args...>, ^^dyn_self_mutref_t<Trait, Args...>>();
    return substitute(^^meta::to_struct, fptrs);
}

}  // namespace anyxx26
