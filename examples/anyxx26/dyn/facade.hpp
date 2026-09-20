#pragma once

#include <examples/anyxx26/dyn/keywords.hpp>
#include <examples/anyxx26/dyn/signature_translation.hpp>
#include <examples/anyxx26/dyn/v_table_layout.hpp>
#include <examples/anyxx26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>
#include <ranges>
#include <string>   
#include <algorithm>

namespace anyxx26 {

struct overload_set_spec {
    std::string name;
    std::vector<v_table_spec> specs;
};
using overload_sets_spec = std::vector<overload_set_spec>;

template <std::meta::info TraitDeclaration>
consteval overload_sets_spec make_overload_sets_specs() {
    overload_sets_spec specs;
    for(auto s : get_v_table_specs<TraitDeclaration>()) {
        if(is_function_or_operator(s)) {
            std::string name{ meta::function_name_of(s.member) };
            auto found = std::ranges::find_if(specs, [&](auto const spec){ return spec.name == name; });
            if(found == specs.end()) {
                specs.push_back(overload_set_spec{ name, {s} });
            } else {
                found->specs.push_back(s);
            }
        }
    }
    return specs;
}

template <typename DynBase, std::meta::info f, typename R, typename... Args>
struct dyn_facade_call {
    template<typename Self>
    R operator()(this Self&& self, Args... args) {
        using base_t = std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, DynBase const, DynBase>;
        auto base = reinterpret_cast<base_t*>(&self);
        using v_table_t = DynBase::v_table_t;
        auto v_table_ptr = base->v_table_;
        using fptrs_t = typename v_table_t::fptrs_t;
        auto fptrs = static_cast<fptrs_t*>(v_table_ptr);
        auto constexpr vf = anyxx26::meta::get_data_member_by_id(^^fptrs_t, meta::function_name_of(f));
        auto x = anyxx::get_proxy_ptr(base->proxy_, v_table_ptr);
        if constexpr(std::same_as<typename[:return_type_of(f):], declaration&>) {
            fptrs->[:vf:](x, std::forward<Args>(args)...);
            return static_cast<typename DynBase::dyn_self_t&>(*base);
        } else {
            return fptrs->[:vf:](x, std::forward<Args>(args)...);
        }
    }
};


template <typename DynBase>
consteval std::meta::info make_dyn_facade_call(std::meta::info m){
    std::vector<std::meta::info> types
    { ^^DynBase
    , reflect_constant(m)
    , translate_facade_return_type(return_type_of(m),^^ typename DynBase::dyn_self_t)
    };
    types.append_range(parameters_of(m)
        | std::views::drop(is_static_member(m) ? 1 : 0)
        | std::views::transform([](auto p){
        return translate_v_table_fptr_param_type(^^typename DynBase::dyn_self_cref_t, ^^typename DynBase::dyn_self_mutref_t, type_of(p));
    })
    );
    return substitute(^^dyn_facade_call, types);
}

template <typename DynBase, std::meta::info TraitDeclaration>
consteval void collect_overload_set_for_name(auto id, std::vector<std::meta::info>& overload_set){
    constexpr auto base = meta::get_type_of_single_public_base<TraitDeclaration>();
    if constexpr(base != std::meta::info{}) {
        collect_overload_set_for_name<DynBase, base>(id, overload_set);
    }
    constexpr auto ctx = std::meta::access_context::current();
    for(auto m : define_static_array(members_of(TraitDeclaration, ctx))) {
        if (is_function(m) && is_user_declared(m)) {
            if(meta::function_name_of(m) == id) {
                overload_set.push_back(make_dyn_facade_call<DynBase>(m));
            }
        }
    }
}

template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <typename DynBase>
consteval std::meta::info dyn_facade_named_overload_set(overload_set_spec const& set){
    std::vector<std::meta::info> overload_set;
    collect_overload_set_for_name<DynBase, ^^ typename DynBase::trait_declaration_t>(set.name, overload_set);
    auto overloaded_operator = substitute(^^overload, overload_set);
    return std::meta::data_member_spec(overloaded_operator, { .name = set.name, .no_unique_address = true });
}

template <std::meta::info TraitDeclaration, typename DynBase>
consteval void collect_dyn_facade_calls(std::vector<std::meta::info>& calls, overload_sets_spec const& overload_sets) {
    for(auto const& set : overload_sets) {
        auto dms = dyn_facade_named_overload_set<DynBase>(set);
        calls.push_back(reflect_constant(dms));
    }
};

template <template <typename, typename, typename...> typename Trait, typename Proxy, typename... Args>
consteval std::meta::info make_dyn_facade() {
     constexpr auto trait_declaration_info = trait_declaration<^^Trait, ^^Args...>();

    [[maybe_unused]] auto overload_sets = make_overload_sets_specs<trait_declaration_info>();
    std::vector<std::meta::info> calls;
    collect_dyn_facade_calls<trait_declaration_info, dyn_base<Trait, Proxy, Args...>>(calls, overload_sets);
    return substitute(^^meta::to_struct, calls);
};

}  // namespace anyxx26
