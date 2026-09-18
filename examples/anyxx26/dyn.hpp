#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <examples/anyxx26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>
#include <format>

namespace anyxx26 {

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct dyn;

struct default_t {};
constexpr static inline default_t defaulted = {};

struct declaration {};
struct model_map {};

struct v_table_data_t {};
constexpr static inline v_table_data_t v_table_data = {};

template <typename V, typename VoidSelf>
using self_const_correct_t = std::conditional_t<
    std::is_const_v<std::remove_pointer_t<std::remove_reference_t<VoidSelf>>>,
    V const, V>;

template<std::meta::info TraitTemplate, typename V, std::meta::info... Args>
consteval std::meta::info trait_model_map(){
    return substitute(TraitTemplate, { ^^V,^^ model_map, Args... });;
}
template<std::meta::info TraitTemplate, std::meta::info... Args>
consteval std::meta::info trait_declaration(){
    return substitute(TraitTemplate, { ^^declaration,^^declaration, Args... });;
}
template<template<typename, typename...> typename TraitTemplate, typename... Args>
using trait_declaration_t = TraitTemplate<declaration, declaration, Args...>;

template <template <typename, typename, typename...> typename Trait, typename... Args>
concept specifies_default_proxy_t = requires { typename trait_declaration_t<Trait, Args...>::default_proxy_t; };

template <template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info compute_default_proxy_t() {
    if constexpr(specifies_default_proxy_t<Trait, Args...>) {
        return ^^typename trait_declaration_t<Trait, Args...>::default_proxy_t;
    } else {
        return ^^anyxx::cref;
    }
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
using default_proxy_t = [:compute_default_proxy_t<Trait, Args...>():];

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_val_t = dyn<Trait, default_proxy_t<Trait, Args...>, Args...>;

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_cref_t = dyn<Trait, anyxx::cref, Args...>;

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_mutref_t = dyn<Trait, anyxx::mutref, Args...>;


template <typename R, typename V>
consteval std::meta::info translate_impl_return_type() {
    if constexpr(^^R == ^^anyxx::self&) {
       return ^^V&;
    } else if constexpr(^^R == ^^anyxx::self) {
        return ^^V;
    } else {
      return ^^R;
    }
}
template <typename R, typename V>
using impl_return_type = [:translate_impl_return_type<R, V>():];


template <std::meta::info SelfValType>
consteval std::meta::info translate_v_table_return_type(std::meta::info R) {
    if (R == ^^anyxx::self&) {
        return ^^void;
    } else if (R == ^^anyxx::self){
        return SelfValType;
    } else {
        return R;
    }
}
static_assert(translate_v_table_return_type<^^int>(^^ anyxx::self&) == ^^void);
static_assert(std::same_as<typename [:translate_v_table_return_type<^^int>(^^anyxx::self&):], void>);

template <std::meta::info Member, typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_member(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return typed_self->[:Member:](std::forward<Args>(args)...);
};
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_parentheses(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return (*typed_self)(std::forward<Args>(args)...);
};
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_plus_plus(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ++(*typed_self);
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_minus_minus(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return --(*typed_self);
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_star(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return *(*typed_self);
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_arrow(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return (*typed_self).operator->();
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_square_brackets(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return (*typed_self)[std::forward<Args>(args)...];
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_plus(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) + ... + std::forward<Args>(args));
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_minus(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) - ... - std::forward<Args>(args));
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_equals_equals(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) == ... == std::forward<Args>(args));
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_exclamation_equals(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) != ... != std::forward<Args>(args));
}   


template <typename R, typename VoidSelf, typename... Args>
using invoke_function_t = R (*)(VoidSelf, Args...);

template <std::meta::info spec, typename Target, typename R, typename VoidSelf, typename... Args>
consteval invoke_function_t<R, VoidSelf, Args...> find_candidate_in_target() {
    constexpr auto ctx = std::meta::access_context::current();
    using self_t = self_const_correct_t<Target, VoidSelf>;
    template for(constexpr auto candidate : define_static_array(members_of(^^Target, ctx))) {
        if constexpr(!is_static_member(candidate) && is_function(candidate)) {
            if constexpr(meta::decorated_name_of(candidate) == meta::decorated_name_of(spec)) {
                if constexpr(std::is_invocable_r_v<R, decltype(&[:candidate:]), self_t, Args...>) {
                    return invoke_member<candidate, self_t, R, VoidSelf, Args...>;
                }
            }
        }
    }
    return {};
}

template <std::meta::info spec, typename V, typename R, typename VoidSelf, typename... Args>
invoke_function_t<impl_return_type<R, V>, VoidSelf, Args...> default_impl() {
    using return_t = impl_return_type<R, V>;
    using self_t = self_const_correct_t<V, VoidSelf>;
    if constexpr(is_class_type(^^V)) {
        if constexpr(constexpr auto candidate = find_candidate_in_target<spec, V, return_t, VoidSelf, Args...>(); candidate) {
            return candidate;
        }
    } 
    if constexpr(meta::is_op_parentheses_spec(spec)) {
        if constexpr(std::is_invocable_r_v<return_t, V, Args...>) {
            return invoke_op_parentheses<self_t, return_t, VoidSelf, Args...>;
        }
    } else {
        if constexpr(!is_operator_function(spec)) {
            throw std::logic_error(std::format("{} has no member function {}.", display_string_of(^^V), display_string_of(spec)));
        } else {
			if constexpr(meta::is_op_spec(spec, std::meta::op_plus_plus)) {
				return invoke_op_plus_plus<self_t, return_t, VoidSelf, Args...>;
			} else if constexpr(meta::is_op_spec(spec, std::meta::op_minus_minus)) {
				return invoke_op_minus_minus<self_t, return_t, VoidSelf, Args...>;
			} else if constexpr(meta::is_op_spec(spec, std::meta::op_star)) {
				return invoke_op_star<self_t, return_t, VoidSelf, Args...>;
			} else if constexpr(meta::is_op_spec(spec, std::meta::op_arrow)) {
                return invoke_op_arrow<self_t, return_t, VoidSelf, Args...>;
			} else if constexpr(meta::is_op_spec(spec, std::meta::op_square_brackets)) {
				return invoke_op_square_brackets<self_t, return_t, VoidSelf, Args...>;
            } else if constexpr(meta::is_op_spec(spec, std::meta::op_plus)) {
                return invoke_op_plus<self_t, return_t, VoidSelf, Args...>;
            } else if constexpr(meta::is_op_spec(spec, std::meta::op_minus)) {
                return invoke_op_minus<self_t, return_t, VoidSelf, Args...>;
            } else if constexpr(meta::is_op_spec(spec, std::meta::op_equals_equals)) {
                return invoke_op_equals_equals<self_t, return_t, VoidSelf, Args...>;
            } else if constexpr(meta::is_op_spec(spec, std::meta::op_exclamation_equals)) {
                return invoke_op_exclamation_equals<self_t, return_t, VoidSelf, Args...>;
            } else {
                throw std::logic_error(std::format("{} not yet implemeted in anyxx.", display_string_of(spec)));
            }
        }
    }
}

consteval std::meta::info make_v_table_fptr_param_type(bool self, std::meta::info p) {
  auto type = type_of(p);
  if (self) {
    if (is_const(remove_reference(type))) {
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

template <std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, typename Param>
consteval std::meta::info translate_v_table_fptr_param_type(){
    if constexpr(^^Param == ^^anyxx::self const&) {
        return dyn_self_cref;
    }
    else if constexpr(^^Param == ^^anyxx::self&) {
        return dyn_self_mutref;
    }
    else {
        return ^^Param;
    }
}

template <std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, typename Param>
using translate_v_table_fptr_param_type_t = [:translate_v_table_fptr_param_type<dyn_self_cref, dyn_self_mutref, Param>():];

template <typename V, typename Param>
consteval std::meta::info translate_impl_fptr_param() {
    if constexpr(^^Param == ^^anyxx::self const&) {
        return ^^V const&;
    } else if constexpr(^^Param == ^^anyxx::self&) {
        return ^^V&;
    } else {
        return ^^Param;
    }
}
template <typename V, typename Param>
using impl_fptr_param_t = [:translate_impl_fptr_param<V, Param>():];

template <typename V, typename Param>
decltype(auto) forward_v_table_fptr_param(auto&& param){
    if constexpr(^^Param == ^^anyxx::self const&) {
        return std::forward<V const&>(*unerase_cast<V>(param));
    } else if constexpr(^^Param == ^^anyxx::self&) {
        return std::forward<V&>(*unerase_cast<V>(param));
    } else {
        return std::forward<Param>(param);
    }
}

template <std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, typename R, typename... Args>
using v_table_fptr_type = R(*)(translate_v_table_fptr_param_type_t<dyn_self_cref, dyn_self_mutref, Args>...);
//using v_table_fptr_type = R(*)(Args...);

template <std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref>
consteval std::meta::info make_v_table_fptr_type(std::meta::info f) {
  std::vector<std::meta::info> types;
  types.push_back(reflect_constant(dyn_self_cref));
  types.push_back(reflect_constant(dyn_self_mutref));
  types.push_back(translate_v_table_return_type<dyn_self_val>(return_type_of(f)));
  add_v_table_fptr_this_param_type(f,types);
  for (auto p : define_static_array(parameters_of(f))) {
    types.push_back(make_v_table_fptr_param_type(types.size() == 3, p));
  }

  return substitute(^^v_table_fptr_type, types);
}

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

consteval std::string_view v_table_name_of(v_table_spec spec) {
    if (is_v_table_data(spec)) {
        return identifier_of(spec.member);
    } else if (is_function(spec)) {
        return identifier_of(spec.member);
    } else {
        return anyxx26::meta::enum_to_string(operator_of(spec.member));
    }
}

template <std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref>
consteval std::meta::info make_v_table_function_data_member_spec(v_table_spec spec) {
    auto ft = make_v_table_fptr_type<dyn_self_val, dyn_self_cref, dyn_self_mutref>(spec.member);
    return std::meta::data_member_spec(ft, { .name = v_table_name_of(spec) });
}

template <std::meta::info TraitDeclaration>
consteval std::vector<v_table_spec> v_table_specs() {
    if constexpr(TraitDeclaration == std::meta::info{}) {
        return {};
    } else {
        static_assert(is_type(TraitDeclaration));
        auto specs = v_table_specs<meta::get_type_of_single_public_base<TraitDeclaration>()>();

        constexpr auto ctx = std::meta::access_context::current();
        for(auto m : members_of(TraitDeclaration, ctx)) {
            if (has_identifier(m) && is_type(m) && annotations_of_with_type(m, ^^v_table_data_t).size() > 0) {
                specs.push_back({TraitDeclaration, m, specs.size()});
            } else if (has_identifier(m) && is_function(m)) {
                specs.push_back({TraitDeclaration, m, specs.size()});
            } else if (is_user_declared(m) && is_operator_function(m)) {
                specs.push_back({TraitDeclaration, m, specs.size()});
            }
        }
        return specs;
    }
}

template <std::meta::info TraitDeclaration, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref>
consteval std::vector<std::meta::info> collect_v_table_members() {

    std::vector<std::meta::info> fptrs;
    template for(constexpr auto spec : define_static_array(v_table_specs<TraitDeclaration>())) {
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
        ^^dyn_self_val_t<Trait, Args...>, ^^dyn_self_cref_t<Trait, Args...>, ^^ dyn_self_mutref_t<Trait, Args...>>();
    return substitute(^^meta::to_struct, fptrs);
}


template <bool default_, std::meta::info m, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, 
    typename V, typename R, typename VoidSelf, typename... Args>
[:translate_v_table_return_type<dyn_self_val>(^^R):] vfimpl(VoidSelf void_self, translate_v_table_fptr_param_type_t<dyn_self_cref, dyn_self_mutref, Args>... args) {
  using return_t = [:translate_v_table_return_type<dyn_self_val>(^^R):];
  if constexpr (default_ || !is_static_member(m)) {
    auto fptr = default_impl<m, V, R, VoidSelf, impl_fptr_param_t<V, Args>...>();
    if constexpr(std::same_as<return_t, void>) {
        fptr(void_self, forward_v_table_fptr_param<V, Args>(args)...);
        return;
    } else {
        return fptr(void_self, forward_v_table_fptr_param<V, Args>(args)...);
    }
  } else {
    using VSelf = self_const_correct_t<V, VoidSelf>;
    if constexpr(std::same_as<return_t, void>) {
        std::forward<R>([:m:](*static_cast<VSelf*>(void_self), forward_v_table_fptr_param<V, Args>(args)...));
        return;
    } else {
        return std::forward<R>([:m:](*static_cast<VSelf*>(void_self), forward_v_table_fptr_param<V, Args>(args)...));
    }
  }
}

template <typename V, std::meta::info implementing_function, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref>
consteval std::meta::info make_vfimpl(std::meta::info interface_function) {
  std::vector<std::meta::info> types;
  bool use_default = annotations_of_with_type(implementing_function, ^^default_t).size() > 0;
  types.push_back(std::meta::reflect_constant(use_default));
  types.push_back(reflect_constant(implementing_function));
  types.push_back(std::meta::reflect_constant(dyn_self_val));
  types.push_back(std::meta::reflect_constant(dyn_self_cref));
  types.push_back(std::meta::reflect_constant(dyn_self_mutref));
  types.push_back(^^V);
  types.push_back(return_type_of(implementing_function));
  add_v_table_fptr_this_param_type(interface_function, types);
  for (auto p : define_static_array(parameters_of(interface_function))) {
    types.push_back(make_v_table_fptr_param_type(types.size() == 7u, p));
  }
  return substitute(^^vfimpl, types);
}

template <template <typename, typename, typename...> typename Trait>
using base_v_table_t = anyxx::observeable::v_table_t;

template <std::meta::info Trait>
consteval std::optional<std::meta::info> find_function_impl_in(std::meta::info interface_function) {
    constexpr auto ctx = std::meta::access_context::current();
    for(auto m : members_of(Trait, ctx)) {
        if (interface_function == m){
            return { m };
        }
        if (is_function(m) && has_identifier(m))
        {
            if (is_function(interface_function) && has_identifier(interface_function)) {
                if (identifier_of(interface_function) == identifier_of(m)) {
                    return { m };
                }
            }
            else if (is_operator_function(interface_function)) {
                if (anyxx26::meta::enum_to_string(operator_of(interface_function)) == identifier_of(m)) {
                    return { m };
                }
            }
        }
    }
    return {};
}

template <std::meta::info TraitTemplate, typename V, std::meta::info... Args>
consteval std::meta::info find_function_impl(std::meta::info interface_function) {

  auto found_in_impl = find_function_impl_in<trait_model_map<TraitTemplate, V, Args...>()>(interface_function);
  if (found_in_impl) {
    return *found_in_impl;
  } 
  auto found_in_base = find_function_impl_in<trait_declaration<TraitTemplate, Args...>()>(interface_function);
  if (found_in_base) {
      return *found_in_base;
  }
  throw std::logic_error("Function not found in impl trait or base trait");
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct v_table;

template <typename VTable, std::meta::info Base, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref,
    typename Concrete, std::meta::info FunctionPointers>
consteval std::meta::info make_set_base_v_table_members();

template <typename VTable, std::meta::info Trait, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, 
    typename Concrete, std::meta::info FunctionPointers, std::meta::info... Args>
void set_v_table_members(VTable* v_table) {
    constexpr auto ctx = std::meta::access_context::current();

    constexpr auto td = trait_declaration<Trait, Args...>();
    constexpr auto base = meta::get_type_of_single_public_base<td>();
    if constexpr(base != std::meta::info{}) {
        if constexpr(has_template_arguments(base) && template_arguments_of(base).size() > 2u) {
            constexpr auto base_set_v_table_fptrs = make_set_base_v_table_members<VTable, base, dyn_self_val, dyn_self_cref, dyn_self_mutref, Concrete, FunctionPointers>();
            [:base_set_v_table_fptrs:] (v_table);
        } else {
            set_v_table_members<VTable, template_of(base), dyn_self_val, dyn_self_cref, dyn_self_mutref, Concrete, FunctionPointers>(v_table);
        }
    }

    template for(constexpr auto interface_m : define_static_array(members_of(trait_declaration<Trait, Args...>(), ctx))) {
        if constexpr(has_identifier(interface_m) && is_type(interface_m) && annotations_of_with_type(interface_m, ^^ v_table_data_t).size() > 0) {
            constexpr auto m = anyxx26::meta::get_data_member_by_id(FunctionPointers, meta::decorated_name_of(interface_m));
            v_table->[:m:] = [:interface_m:]::template init<Concrete>(v_table);
        }
        if constexpr((has_identifier(interface_m) && is_function(interface_m))
            || (is_user_declared(interface_m) && is_operator_function(interface_m))) {
            constexpr auto f = anyxx26::meta::get_data_member_by_id(FunctionPointers, meta::decorated_name_of(interface_m));
            constexpr auto m = find_function_impl<Trait, Concrete, Args...>(interface_m);
            v_table->[:f:] = [:make_vfimpl<Concrete, m, dyn_self_val, dyn_self_cref, dyn_self_mutref>(interface_m):];
        }
    }
}

template <typename VTable, std::meta::info Base, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref,
    typename Concrete, std::meta::info FunctionPointers>
consteval std::meta::info make_set_base_v_table_members() {
    constexpr auto base_trait_template = template_of(Base);
    std::vector<std::meta::info> function_params = {
        ^^VTable,
        reflect_constant(base_trait_template),
        reflect_constant(dyn_self_val),
        reflect_constant(dyn_self_cref),
        reflect_constant(dyn_self_mutref),
        ^^Concrete,
        reflect_constant(FunctionPointers),
    };
	static_assert(template_arguments_of(Base).size() > 2u);
    function_params.append_range(template_arguments_of(Base) | std::views::drop(2) | std::views::transform([](std::meta::info i){ return reflect_constant(i); }));
    return substitute(^^set_v_table_members, function_params);
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct v_table
    : base_v_table_t<Trait>,
    [: make_v_table_members_type<Trait, Args...>():] {
    using v_table_t = v_table;
	using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
    using  fptrs_t = [:make_v_table_members_type<Trait, Args...>():];
    template <typename Concrete>
    v_table(std::in_place_type_t<Concrete> concrete)
        : base_v_table_t<Trait>(concrete) {
        set_v_table_members<v_table_t, ^^Trait, ^^dyn_self_val_t<Trait, Args...>, ^^dyn_self_cref_t<Trait, Args...>, ^^dyn_self_mutref_t<Trait, Args...>, Concrete, ^^fptrs_t, ^^Args...>(this);
    }
};

template <template <typename, typename, typename...> typename Trait, typename V, typename... Args>
v_table<Trait, Args...>* get_v_table_instance() {
  static v_table<Trait, Args...> instance(std::in_place_type<V>);
  return &instance;
};

template <typename Dyn>
concept is_dyn = anyxx::is_any<Dyn> && 
    requires { typename Dyn::trait_declaration_t; };

template <typename ToVtable, typename FromVTable>
	requires std::derived_from<typename FromVTable::trait_declaration_t, typename ToVtable::trait_declaration_t>
ToVtable* v_table_cast(FromVTable* from) {
	auto void_p = static_cast<void*>(from);
	return static_cast<ToVtable*>(void_p);
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
concept has_deduced_typenames = requires { typename Trait<declaration, declaration, Args...>::typenames; };

struct empty_t {};

template <template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info compute_deduced_typenames() {
	if constexpr(has_deduced_typenames<Trait, Args...>) {
	    return ^^ typename Trait<declaration, declaration, Args...>::typenames;
	} else {
		return ^^empty_t;
    }
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
using deduced_typenames = [:compute_deduced_typenames<Trait, Args...>():];


template <template <typename, typename, typename...> typename Trait, typename Proxy, typename... Args>
struct dyn_base : deduced_typenames<Trait, Args...> {
  using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
  using dyn_self_t = dyn<Trait, Proxy, Args...>;
  using proxy_t = Proxy;
  using proxy_trait_t = anyxx::proxy_trait<proxy_t>;
  using void_t = typename proxy_trait_t::void_t;
  using v_table_t = v_table<Trait, Args...>;

  v_table_t* v_table_;
  proxy_t proxy_{};

  dyn_base()
    requires proxy_trait_t::allow_any_default_constructibile
  {}

  template <typename ConstructedWith>
  explicit(false) dyn_base(ConstructedWith&& constructed_with)  // NOLINT
    requires anyxx::constructibile_for<ConstructedWith, proxy_t,
                                       dyn_base<Trait, proxy_t, Args...>>
      : v_table_(get_v_table_instance<Trait, std::decay_t<ConstructedWith>, Args...>()),
        proxy_(anyxx::erased<proxy_t>(
            std::forward<ConstructedWith>(constructed_with))) {}

  template <typename V>
    requires(!anyxx::is_lifetime_bound<proxy_t>)
  dyn_base(std::in_place_t, V&& v)
      : v_table_(get_v_table_instance<Trait, V, Args...>()),
        proxy_(proxy_trait_t::construct_in_place(std::forward<V>(v))) {}

  template <typename T, typename... ConstructWithArgs>
    requires(!anyxx::is_lifetime_bound<proxy_t>)
  dyn_base(std::in_place_type_t<T>, ConstructWithArgs&&... args)
      : v_table_(get_v_table_instance<Trait, T, Args...>()),
        proxy_(proxy_trait_t::template construct_type_in_place<T>(
            std::forward<ConstructWithArgs>(args)...)) {}

  ~dyn_base() { proxy_trait_t::destroy(proxy_, v_table_); }

  dyn_base(const dyn_base& other)
    requires(anyxx::can_copy_construct_from<proxy_trait_t, v_table_t>)
      : v_table_(other.v_table_) {
    proxy_trait_t::copy_construct_from(proxy_, nullptr, other.proxy_,
                                       other.v_table_);
  }
  dyn_base& operator=(dyn_base const& other)
    requires(anyxx::can_copy_construct_from<proxy_trait_t, v_table_t>)
  {
    if (this == &other) return *this;
    auto v_table_ptr = v_table_;
    proxy_trait_t::copy_construct_from(proxy_, v_table_ptr, other.proxy_,
                                       other.v_table_);
    return *this;
  }
  dyn_base(dyn_base&& other) noexcept  // NOLINT(noExplicitConstructor)
    requires(anyxx::moveable_from<proxy_t, proxy_t>)
      : dyn_base(std::move(other.proxy_), release_v_table(other)) {}
  dyn_base& operator=(dyn_base&& other) noexcept
    requires(anyxx::moveable_from<proxy_t, proxy_t>)
  {
    proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_), other.v_table_);
    v_table_ = release_v_table(other);
    return *this;
  }

  template <is_dyn Other>
  explicit(false) dyn_base(const Other& other)  // NOLINT(noExplicitConstructor)
      requires(anyxx::proxy_borrowable_from<proxy_t, typename Other::proxy_t, typename Other::v_table_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
      : v_table_(v_table_cast<v_table_t>(other.v_table_)),
      proxy_(borrow_proxy_as<proxy_t>(other.proxy_, other.v_table_)) {
  }
  template <is_dyn Other>
  dyn_base& operator=(Other const& other)
      requires(anyxx::proxy_borrowable_from<proxy_t, typename Other::proxy_t, typename Other::v_table_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
  {
      v_table_ = v_table_cast<v_table_t>(other.v_table_);
      proxy_ = anyxx::borrow_proxy_as<proxy_t>(other.proxy_, other.v_table_);
      return *this;
  }

  template <anyxx::is_proxy OtherErasedData>
      requires(anyxx::moveable_from<proxy_t, OtherErasedData>)
  explicit dyn_base(OtherErasedData&& proxy, v_table_t* v_table) noexcept
      : v_table_(v_table) {
      proxy_trait_t::move_to(proxy_, nullptr, std::move(proxy), v_table);
  }
  template <is_dyn Other>
  explicit(false) dyn_base(Other&& other) noexcept  // NOLINT(noExplicitConstructor)
      requires(anyxx::moveable_from<proxy_t, typename Other::proxy_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
      : dyn_base(std::move(other.proxy_), v_table_cast<v_table_t>(release_v_table(other))) {
  }
  template <is_dyn Other>
  dyn_base& operator=(Other&& other) noexcept
      requires(anyxx::moveable_from<proxy_t, typename Other::proxy_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
  {
      proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_), other.v_table_);
      v_table_ = v_table_cast<v_table_t>(release_v_table(other));
      return *this;
  }

  template <typename Self, typename... Params>
  decltype(auto) operator()(this Self&& self, Params&&... params) {
      return self.op_parentheses(std::forward<Params>(params)...);
  }

#define __dyn_OP(function, op) \
  template <typename Self, typename... Params> \
  decltype(auto) operator op (this Self&& self, Params&&... params) { \
      return self.function(std::forward<Params>(params)...); \
  }
#define __dyn_OP0(function, op) \
  template <typename Self> \
  decltype(auto) operator op (this Self&& self) { \
      return self.function(); \
  }

    __dyn_OP(op_square_brackets, [])
    __dyn_OP0(op_tilde, ~)
    __dyn_OP0(op_exclamation, !)
    __dyn_OP(op_plus, +)
    __dyn_OP(op_minus, -)
    __dyn_OP(op_star, *)
    __dyn_OP(op_slash, /) 
    __dyn_OP(op_percent, %)
    __dyn_OP(op_caret, ^)
    __dyn_OP(op_ampersand, &)
    __dyn_OP(op_pipe, |)
    __dyn_OP(op_plus_equals, +=)
    __dyn_OP(op_minus_equals, -=)
    __dyn_OP(op_star_equals, *=)
    __dyn_OP(op_slash_equals, /=)
    __dyn_OP(op_percent_equals, %=)
    __dyn_OP(op_caret_equals, ^=)
    __dyn_OP(op_ampersand_equals, &=)
    __dyn_OP(op_pipe_equals, |=)
    __dyn_OP(op_equals_equals, ==)
    __dyn_OP(op_exclamation_equals, != )
    __dyn_OP(op_less, <)
    __dyn_OP(op_greater, >)
    __dyn_OP(op_less_equals, <=)
    __dyn_OP(op_greater_equals, >=)
    __dyn_OP(op_spaceship, <=>)
    __dyn_OP(op_less_less, <<)
    __dyn_OP(op_greater_greater, >>)
    __dyn_OP(op_less_less_equals, <<=)
    __dyn_OP(op_greater_greater_equals, >>=)
    __dyn_OP0(op_plus_plus, ++)
    __dyn_OP0(op_minus_minus, --)

#undef __dyn_OP
#undef __dyn_OP0

  template <typename Self>
    decltype(auto) operator++(this Self&& self, int) {
        std::decay_t<Self> old = self;
        ++self;
        return old;
    }  
  template <typename Self>
  decltype(auto) operator--(this Self&& self, int) {
      std::decay_t<Self> old = self;
      --self;
      return old;
  }

  friend auto release_v_table(dyn_base& self) { return std::exchange(self.v_table_, nullptr); }
};

template <typename DynBase, std::meta::info f>
struct dyn_facade_call {
  template <typename Self, typename... Args>
  decltype(auto) operator()(this Self&& self, Args&&... args) {
    using base_t = std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, DynBase const, DynBase>;
    auto base = reinterpret_cast<base_t*>(&self);
    using v_table_t = DynBase::v_table_t;
    auto v_table_ptr = base->v_table_;
    using fptrs_t = typename v_table_t::fptrs_t;
    auto fptrs = static_cast<fptrs_t*>(v_table_ptr);
    auto constexpr vf = anyxx26::meta::get_data_member_by_id(^^fptrs_t, meta::decorated_name_of(f));
    auto x = anyxx::get_proxy_ptr(base->proxy_, v_table_ptr);
    if constexpr(std::same_as<typename [:return_type_of(f):], anyxx::self&>) {
        fptrs->[:vf:](x, std::forward<Args>(args)...);
        return static_cast<typename DynBase::dyn_self_t&>(*base);
    } else {
        return fptrs->[:vf:](x, std::forward<Args>(args)...);
    }
  }
};

template <typename DynBase, std::meta::info f, auto id>
consteval std::meta::info dyn_facade_call_data_member_spec(){
    using dyn_facade_call_t = dyn_facade_call<DynBase, f>;
    constexpr std::meta::info call_meta = ^^dyn_facade_call_t;
    return std::meta::data_member_spec(
        call_meta, { .name = id, .no_unique_address = true });
}

template <std::meta::info TraitDeclaration, typename DynBase>
consteval void collect_dyn_facade_calls(std::vector<std::meta::info>& calls) {
    constexpr auto base = meta::get_type_of_single_public_base<TraitDeclaration>();
    if constexpr(base != std::meta::info{}) {
        collect_dyn_facade_calls<base, DynBase>(calls);
    }
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m :
        define_static_array(members_of(TraitDeclaration, ctx))) {
        if constexpr (is_function(m)) {
            if constexpr(has_identifier(m)) {
                auto dms = dyn_facade_call_data_member_spec<DynBase, m, define_static_string(identifier_of(m))>();
                calls.push_back(reflect_constant(dms));
            }
            else if constexpr(is_user_declared(m) && is_operator_function(m)) {
                auto dms = dyn_facade_call_data_member_spec<DynBase, m, define_static_string(meta::enum_to_string(operator_of(m)))>();
                calls.push_back(reflect_constant(dms));
            }
        }
    }
};

template <template <typename, typename, typename...> typename Trait, typename Proxy, typename... Args>
consteval std::meta::info make_dyn_facade() {

  std::vector<std::meta::info> calls;
  collect_dyn_facade_calls<trait_declaration<^^Trait, ^^Args...>(), dyn_base<Trait, Proxy, Args...>>(calls);
  return substitute(^^meta::to_struct, calls);
};

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct dyn : dyn_base<Trait, Args...>, [:make_dyn_facade<Trait, Args...>():] {
  using dyn_base<Trait, Args...>::dyn_base;
};

/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, typename Dyn>
    requires is_dyn<Dyn>
inline auto unerase_cast(Dyn const& o) {
    return unerase_cast_if<U>(o.proxy_, o.v_table_);
}
/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, typename Dyn>
    requires is_dyn<Dyn>
inline auto unerase_cast_if(Dyn const& o) {
    return unerase_cast_if<U>(o.proxy_, o.v_table_);
}

struct type_info_{
    using type = std::type_info const*;
    template<typename Concrete>
    static std::type_info const* init(auto){ return &typeid(Concrete); }
};
struct is_derived_from_{
    using type = anyxx::is_derived_from_t;
    template<typename Concrete, typename VTable>
    static auto const* init(VTable* v_table ){ 
        return +[](const std::type_info& from) {
            return VTable::static_is_derived_from(from);
        };
    }
};
struct meta_data_{
    using type = anyxx::meta_data*;
    template<typename Concrete>
    static auto init(auto){ return nullptr; };
};
struct model_size {
    using type = anyxx::model_size_t;
    template<typename Concrete>
    static auto init(auto){ return anyxx::compute_model_size<Concrete>(); };
};
struct copy_constructor {
    using type = anyxx::copy_constructor_t;
    template<typename Concrete>
    static auto init(auto){ 
        return []([[maybe_unused]] anyxx::mutable_void placement,
                  [[maybe_unused]] anyxx::const_void from) {
            return anyxx::invoke_copy_constructor<Concrete>(placement, from);
        };
    }
};
struct move_constructor {
    using type = anyxx::move_constructor_t;
    template<typename Concrete>
    static auto init(auto){
        return []([[maybe_unused]] anyxx::mutable_void placement,
            [[maybe_unused]] anyxx::mutable_void from) {
            return anyxx::invoke_move_constructor<Concrete>(placement, from);
        };
    }
};
struct delete_ {
    using type = anyxx::delete_t;
    template<typename Concrete>
    static auto init(auto){
        return [](anyxx::mutable_void data) {
            if(data) delete static_cast<Concrete*>(data);
        };
    }
};
struct destructor {
    using type = anyxx::destructor_t;
    template<typename Concrete>
    static auto init(auto){
        return [](anyxx::mutable_void data) {
            std::destroy_at(static_cast<Concrete*>(data));
        };
    }
};

// clang-format off
template <typename Self, typename = anyxx26::declaration>
struct moveable {
    using model_size [[= v_table_data]] = anyxx26::model_size;
    using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
    using destructor [[= v_table_data]] = anyxx26::destructor;
    using default_proxy_t = anyxx::val<>;
};
    template <typename Self, typename = anyxx26::declaration>
    struct copyable : moveable<Self> {
        using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
    };
template <typename Self, typename = anyxx26::declaration>
struct save_observable {
    using type_info_ [[= v_table_data]] = anyxx26::type_info_;
};
    template <typename Self, typename = anyxx26::declaration>
    struct save_moveable : save_observable<Self> {
        using model_size [[= v_table_data]] = anyxx26::model_size;
        using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
        using destructor [[= v_table_data]] = anyxx26::destructor;
        using default_proxy_t = anyxx::val<>;
    };
        template <typename Self, typename = anyxx26::declaration>
        struct save_copyable : save_moveable<Self> {
            using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
        };
    template <typename Self, typename = anyxx26::declaration>
    struct dynamic_castable : save_observable<Self> {
        using is_derived_from_ [[= v_table_data]] = anyxx26::is_derived_from_;
        using meta_data_ [[= v_table_data]] = anyxx26::meta_data_;
    };
        template <typename Self, typename = anyxx26::declaration>
        struct dynamic_deletable : dynamic_castable<Self> {
            using delete_ [[= v_table_data]] = anyxx26::delete_;
            using default_proxy_t = anyxx::shared;
        };
            template <typename Self, typename = anyxx26::declaration>
            struct dynamic_smart_ptr : dynamic_deletable<Self> {
                using model_size [[= v_table_data]] = anyxx26::model_size;
                using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
            };
        template <typename Self, typename = anyxx26::declaration>
        struct dynamic_moveable : dynamic_castable<Self> {
            using model_size [[= v_table_data]] = anyxx26::model_size;
            using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
            using destructor [[= v_table_data]] = anyxx26::destructor;
            using default_proxy_t = anyxx::val<>;
        };
            template <typename Self, typename = anyxx26::declaration>
            struct dynamic_copyable : dynamic_moveable<Self> {
                using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
            };
// clang-format on

}  // namespace anyxx26
