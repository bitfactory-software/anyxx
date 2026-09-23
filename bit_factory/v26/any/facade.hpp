#pragma once

#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/any/signature_translation.hpp>
#include <bit_factory/v26/trait_interface/overload_set_spec.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>
#include <ranges>
#include <string>   
#include <algorithm>

namespace anyxx26 {


template <typename AnyBase, bool const_, std::meta::info f, std::size_t v_table_index, typename R, typename... Args>
R dyn_facade_call(auto&& self, Args... args) {
    consteval{
        if constexpr(!const_ && std::is_const_v<std::remove_reference_t<decltype(self)>>) {
            std::string msg = std::string{ "mutable " } + std::string{ display_string_of(f) } + " cannot be called on const Self";
            throw std::meta::exception(msg, dealias(^^std::decay_t<decltype(self)>));
        }
    }
    auto void_self = static_cast<std::conditional_t<const_, void const*, void*>>(&self);
    auto base = static_cast<std::conditional_t<const_, AnyBase const*, AnyBase*>>(void_self);
    using v_table_t = AnyBase::v_table_t;
    auto v_table_ptr = base->v_table_;
    using fptrs_t = typename v_table_t::fptrs_t;
    auto fptrs = static_cast<fptrs_t*>(v_table_ptr);
    auto constexpr vf = anyxx26::meta::get_data_member_by_id(^^fptrs_t, v_table_name_of(f, v_table_index));
    auto x = anyxx::get_proxy_ptr(base->proxy_, v_table_ptr);
    if constexpr(std::same_as<typename[:return_type_of(f):], declaration&>) {
        if constexpr(const_) {
            consteval {
                std::string msg = std::string{"const "} + std::string{display_string_of(f)} + " cannot return mutable self reference.";
                throw std::meta::exception(msg, dealias(^^std::decay_t<decltype(self)>));
            }
        } else {
            fptrs->[:vf:](x, std::forward<Args>(args)...);
            return static_cast<typename AnyBase::dyn_self_t&>(*base);
        }
    } else {
        return fptrs->[:vf:](x, std::forward<Args>(args)...);
    }
}

template <typename AnyBase, std::meta::info f, std::size_t v_table_index, typename R, typename... Args>
struct const_dyn_facade_call {
    template <typename Self>
    R operator()(this Self const& self, Args... args) {
        return dyn_facade_call<AnyBase, true, f, v_table_index, R, Args...>(self, std::forward<Args>(args)...);
    }
};
template <typename AnyBase, std::meta::info f, std::size_t v_table_index, typename R, typename... Args>
struct mutable_dyn_facade_call {
    template<typename Self>
    R operator()(this Self& self, Args... args) {
        return dyn_facade_call<AnyBase, false, f, v_table_index, R, Args...>(self, std::forward<Args>(args)...);
    }
};


template <typename AnyBase>
consteval std::meta::info make_dyn_facade_call(interface_spec const& spec){
    std::vector<std::meta::info> types
    { ^^AnyBase
    , reflect_constant(spec.member)
    , std::meta::reflect_constant(spec.index)
    , translate_facade_return_type(return_type_of(spec.member),^^ typename AnyBase::dyn_self_t)
    };
    types.append_range(parameters_of(spec.member)
        | std::views::drop(is_static_member(spec.member) ? 1 : 0)
        | std::views::transform([](auto p){
            return translate_v_table_fptr_param_type(^^typename AnyBase::dyn_self_cref_t, ^^typename AnyBase::dyn_self_mutref_t, type_of(p));
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
        if( !anyxx::is_const_data<typename AnyBase::proxy_t>) {
            return substitute(^^mutable_dyn_facade_call, types);
        }
    }
    return std::meta::info{};
}

template <typename AnyBase>
consteval std::meta::info dyn_facade_named_overload_set(overload_set_spec const& spec){
    std::vector<std::meta::info> overload_set;
    for(auto overload : spec.specs) {
      if (auto call = make_dyn_facade_call<AnyBase>(overload); call != std::meta::info{}) {
        overload_set.push_back(make_dyn_facade_call<AnyBase>(overload));
      }
    }
    auto overloaded_call_operator = substitute(^^meta::overload, overload_set);
    return std::meta::data_member_spec(overloaded_call_operator, { .name = spec.name, .no_unique_address = true });
}

template <template <typename, typename, typename...> typename Trait, typename Proxy, typename... Args>
consteval auto make_dyn_facade_overloaded_calls() {
    std::vector<std::meta::info> calls;
    for(auto const& set : make_overload_sets_specs<Trait, Args...>()) {
        calls.push_back(reflect_constant(dyn_facade_named_overload_set<any_base<Trait, Proxy, Args...>>(set)));
    }
    return calls;
};

template <template <typename, typename, typename...> typename Trait, typename Proxy, typename... Args>
consteval std::meta::info make_dyn_facade() {
    return substitute(^^meta::to_struct, make_dyn_facade_overloaded_calls<Trait, Proxy, Args...>());
};

}  // namespace anyxx26
