#pragma once

#include <meta>
#include <algorithm>
#include <examples/anyxx26/dyn/keywords.hpp>
#include <examples/anyxx26/meta/utilities.hpp>

namespace anyxx26 {

struct v_table_spec {
  std::meta::info declaration_trait;
  std::meta::info member;
  std::size_t index;
};

consteval bool is_v_table_data(v_table_spec spec) {
    return has_identifier(spec.member) && is_type(spec.member) && annotations_of_with_type(spec.member, ^^v_table_data_t).size() > 0;
}
consteval bool is_function(v_table_spec spec) {
    return has_identifier(spec.member) && is_function(spec.member);
}
consteval bool is_operator(v_table_spec spec) {
    return is_user_declared(spec.member) && is_operator_function(spec.member);
}
consteval bool is_function_or_operator(v_table_spec spec) {
    return is_function(spec) || is_operator(spec);
}

consteval std::string v_table_name_of(v_table_spec spec) {
    if (is_v_table_data(spec)) {
        return std::string{ identifier_of(spec.member) };
    } else {
        return std::string{ meta::function_name_of(spec.member) };
    }
}

consteval std::vector<v_table_spec> get_v_table_specs(std::meta::info declaration_trait) {
    if (declaration_trait == std::meta::info{}) {
        return {};
    } else {
        declaration_trait = dealias(declaration_trait);
        auto specs = get_v_table_specs(meta::get_type_of_single_public_base(declaration_trait));

        constexpr auto ctx = std::meta::access_context::current();
        for(auto m : members_of(declaration_trait, ctx)) {
            if (has_identifier(m) && is_type(m) && annotations_of_with_type(m, ^^v_table_data_t).size() > 0) {
                specs.push_back({declaration_trait, m, specs.size()});
            } else if (has_identifier(m) && is_function(m)) {
                specs.push_back({declaration_trait, m, specs.size()});
            } else if (is_user_declared(m) && is_operator_function(m)) {
                specs.push_back({declaration_trait, m, specs.size()});
            }
        }
        return specs;
    }
}

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

template <std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref>
consteval std::meta::info make_v_table_function_data_member_spec(v_table_spec spec) {
    auto ft = make_v_table_fptr_type<dyn_self_val, dyn_self_cref, dyn_self_mutref>(spec.member);
    return std::meta::data_member_spec(ft, { .name = v_table_name_of(spec) });
}

template <std::meta::info TraitDeclaration, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref>
consteval std::vector<std::meta::info> collect_v_table_members() {

    std::vector<std::meta::info> fptrs;
    template for(constexpr auto spec : define_static_array(get_v_table_specs(TraitDeclaration))) {
        if constexpr(is_v_table_data(spec)) {
            using type = [:spec.member:]::type;
            auto dms = std::meta::data_member_spec(dealias(^^type), { .name = v_table_name_of(spec) });
            fptrs.push_back(reflect_constant(dms));
        }
        else {
            auto dms = make_v_table_function_data_member_spec<dyn_self_val, dyn_self_cref, dyn_self_mutref>(spec);
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
