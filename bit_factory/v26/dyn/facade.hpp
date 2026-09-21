#pragma once

#include <bit_factory/v26/dyn/keywords.hpp>
#include <bit_factory/v26/dyn/signature_translation.hpp>
#include <bit_factory/v26/dyn/make_v_table_members_type.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
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
    for(auto s : get_v_table_specs(TraitDeclaration)) {
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

template <typename DynBase, std::meta::info f, std::size_t v_table_index, typename R, typename... Args>
struct const_dyn_facade_call {
    template<typename Self>
    R operator()(this Self const& self, Args... args) {
        using base_t = std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, DynBase const, DynBase>;
        auto base = reinterpret_cast<const base_t*>(&self);
        using v_table_t = DynBase::v_table_t;
        auto v_table_ptr = base->v_table_;
        using fptrs_t = typename v_table_t::fptrs_t;
        auto fptrs = static_cast<fptrs_t*>(v_table_ptr);
        auto constexpr vf = anyxx26::meta::get_data_member_by_id(^^fptrs_t, v_table_name_of(f, v_table_index));
        auto x = anyxx::get_proxy_ptr(base->proxy_, v_table_ptr);
        if constexpr(std::same_as<typename[:return_type_of(f):], declaration&>) {
            fptrs->[:vf:](x, std::forward<Args>(args)...);
            return static_cast<typename DynBase::dyn_self_t const&>(*base);
        } else {
            return fptrs->[:vf:](x, std::forward<Args>(args)...);
        }
    }
};
template <typename DynBase, std::meta::info f, std::size_t v_table_index, typename R, typename... Args>
struct mutable_dyn_facade_call {
    template<typename Self>
    R operator()(this Self& self, Args... args) {
        using base_t = std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, DynBase const, DynBase>;
        auto base = reinterpret_cast<base_t*>(&self);
        using v_table_t = DynBase::v_table_t;
        auto v_table_ptr = base->v_table_;
        using fptrs_t = typename v_table_t::fptrs_t;
        auto fptrs = static_cast<fptrs_t*>(v_table_ptr);
        auto constexpr vf = anyxx26::meta::get_data_member_by_id(^^fptrs_t, v_table_name_of(f, v_table_index));
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
consteval std::meta::info make_dyn_facade_call(v_table_spec spec){
    std::vector<std::meta::info> types
    { ^^DynBase
    , reflect_constant(spec.member)
    , std::meta::reflect_constant(spec.index)
    , translate_facade_return_type(return_type_of(spec.member),^^ typename DynBase::dyn_self_t)
    };
    types.append_range(parameters_of(spec.member)
        | std::views::drop(is_static_member(spec.member) ? 1 : 0)
        | std::views::transform([](auto p){
            return translate_v_table_fptr_param_type(^^typename DynBase::dyn_self_cref_t, ^^typename DynBase::dyn_self_mutref_t, type_of(p));
            })
        );
    //if (is_function(spec.member) && is_static_member(spec.member)) {
    //        if(is_const(type_of(spec.member))) {
    //            throw std::meta::exception(std::string{ "const: " } + std::string{display_string_of(type_of(spec.member))}, spec.member);
    //        } else { 
    //            throw std::meta::exception(std::string{ "non-const: " } + std::string{ display_string_of(type_of(spec.member)) }, spec.member);
    //        }
    //}
    if (is_const_function(spec.member)) {
        return substitute(^^const_dyn_facade_call, types);
    } else {
        if( !anyxx::is_const_data<typename DynBase::proxy_t>) {
            return substitute(^^mutable_dyn_facade_call, types);
        }
    }
    return std::meta::info{};
}

template<class... Ts>
struct overload : Ts... {
    using Ts::operator()...;
};
template <typename DynBase>
consteval std::meta::info dyn_facade_named_overload_set(overload_set_spec const& spec){
    std::vector<std::meta::info> overload_set;
    for(auto overload : spec.specs) {
      if (auto call = make_dyn_facade_call<DynBase>(overload); call != std::meta::info{}) {
        overload_set.push_back(make_dyn_facade_call<DynBase>(overload));
      }
    }
    auto overloaded_operator = substitute(^^overload, overload_set);
    return std::meta::data_member_spec(overloaded_operator, { .name = spec.name, .no_unique_address = true });
}

template <typename DynBase>
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
    collect_dyn_facade_calls<dyn_base<Trait, Proxy, Args...>>(calls, overload_sets);
    return substitute(^^meta::to_struct, calls);
};

}  // namespace anyxx26
