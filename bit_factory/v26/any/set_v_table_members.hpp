#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/any/facade.hpp>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/any/signature_translation.hpp>
#include <bit_factory/v26/any/make_v_table_members_type.hpp>
#include <bit_factory/v26/trait_translation/get_implementation_member.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>

namespace anyxx26 {

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
    if constexpr(sizeof...(Args) == 0) {
        return *(*typed_self);
    } else {
        return ((*typed_self) * ... * std::forward<Args>(args));
    }
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_arrow(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return std::to_address(*typed_self);
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_square_brackets(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return (*typed_self)[std::forward<Args>(args)...];
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_plus(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    if constexpr(sizeof...(Args) == 0) {
        return +(*typed_self);
    } else {
        return ((*typed_self) + ... + std::forward<Args>(args));
    }
}
template <typename TypedSelf, typename R, typename VoidSelf, typename... Args>
R invoke_op_minus(VoidSelf self, Args... args) {
    auto typed_self = static_cast<TypedSelf*>(self);
    if constexpr(sizeof...(Args) == 0) {
        return -(*typed_self);
    } else {
        return ((*typed_self) - ... - std::forward<Args>(args));
    }
}
template <typename TypedSelf, typename R, typename VoidSelf, typename Arg>
R invoke_op_equals_equals(VoidSelf self, Arg arg) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) == std::forward<Arg>(arg));
}
template <typename TypedSelf, typename R, typename VoidSelf, typename Arg>
R invoke_op_exclamation_equals(VoidSelf self, Arg arg) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) != std::forward<Arg>(arg));
}   
template <typename TypedSelf, typename R, typename VoidSelf, typename Arg>
R invoke_op_less(VoidSelf self, Arg arg) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) < std::forward<Arg>(arg));
}
template <typename TypedSelf, typename R, typename VoidSelf, typename Arg>
R invoke_op_less_equals(VoidSelf self, Arg arg) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) <= std::forward<Arg>(arg));
}
template <typename TypedSelf, typename R, typename VoidSelf, typename Arg>
R invoke_op_greater(VoidSelf self, Arg arg) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) > std::forward<Arg>(arg));
}
template <typename TypedSelf, typename R, typename VoidSelf, typename Arg>
R invoke_op_greater_equals(VoidSelf self, Arg arg) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return ((*typed_self) >= std::forward<Arg>(arg));
}
template <typename TypedSelf, typename R, typename VoidSelf, typename Arg>
R invoke_op_plus_equals(VoidSelf self, Arg arg) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return (*typed_self) += arg;
}
template <typename TypedSelf, typename R, typename VoidSelf, typename Arg>
R invoke_op_minus_equals(VoidSelf self, Arg arg) {
    auto typed_self = static_cast<TypedSelf*>(self);
    return (*typed_self) -= arg;
}

template <typename R, typename VoidSelf, typename... Args>
using invoke_function_t = R (*)(VoidSelf, Args...);

template <std::meta::info spec, typename Target, typename R, typename VoidSelf, typename... Args>
consteval invoke_function_t<R, VoidSelf, Args...> find_candidate_in_target() {
    constexpr auto ctx = std::meta::access_context::current();
    using self_t = self_const_correct_t<Target, VoidSelf>;
    template for(constexpr auto candidate : define_static_array(members_of(^^Target, ctx))) {
        if constexpr(!is_static_member(candidate) && is_function(candidate)) {
            if constexpr(meta::function_name_of(candidate) == meta::function_name_of(spec)) {
                if constexpr(std::is_invocable_r_v<R, decltype(&[:candidate:]), self_t, Args...>) {
                    return invoke_member<candidate, self_t, R, VoidSelf, Args...>;
                }
            }
        }
    }
    return {};
}

template <std::meta::info spec, typename V, typename R, typename VoidSelf, typename... Args>
consteval invoke_function_t<impl_return_type<R, V>, VoidSelf, Args...> default_impl() {
    using return_t = impl_return_type<R, V>;
    using self_t = self_const_correct_t<V, VoidSelf>;
    if constexpr(is_class_type(^^V)) {
        if constexpr(constexpr auto candidate = find_candidate_in_target<spec, V, return_t, VoidSelf, Args...>(); candidate) {
            return candidate;
        }
    } 
    if constexpr(meta::is_op_parentheses_spec(spec)) {
        if constexpr(std::is_invocable_r_v<return_t, self_t, Args...>) {
            return invoke_op_parentheses<self_t, return_t, VoidSelf, Args...>;
        //} else {
        //    std::string msg{display_string_of(^^V)};
        //    msg += " has no implementation for " + std::string{display_string_of(spec)};
        //    throw std::meta::exception(msg, ^^V);
        }
    } else {
        if constexpr(!is_operator_function(spec)) {
            std::string msg{ display_string_of(^^V) };
            msg += " has no member function " + std::string{display_string_of(spec)};
            throw std::meta::exception(msg, ^^V);
        } else {
#define __RETURN_OP(op_name) \
			if constexpr(meta::is_op_spec(spec, std::meta::op_##op_name)) { \
                return invoke_op_##op_name<self_t, return_t, VoidSelf, Args...>; \
            }

            __RETURN_OP(plus_plus)
			else __RETURN_OP(minus_minus)
			else __RETURN_OP(star)
			else __RETURN_OP(arrow)
			else __RETURN_OP(square_brackets)
            else __RETURN_OP(plus)
            else __RETURN_OP(minus)
            else __RETURN_OP(equals_equals)
            else __RETURN_OP(exclamation_equals)
            else __RETURN_OP(less)
            else __RETURN_OP(less_equals)
            else __RETURN_OP(greater)
            else __RETURN_OP(greater_equals)
            else __RETURN_OP(plus_equals)
            else __RETURN_OP(minus_equals)

#undef __RETURN_OP
            else {
                std::string msg{ display_string_of(spec) };
                msg += " not yet implemeted in anyxx " + std::string{ display_string_of(spec) };
                throw std::meta::exception(msg, spec);
            }
        }
    }
}

template <bool default_, std::meta::info m, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, 
    typename V, typename R, typename VoidSelf, typename... Args>
[:translate_v_table_return_type<dyn_self_val>(^^R):] vfimpl(VoidSelf void_self, translate_v_table_fptr_param_type_t<dyn_self_cref, dyn_self_mutref, Args>... args) {
  using return_t = [:translate_v_table_return_type<dyn_self_val>(^^R):];
  if constexpr (default_ || !is_static_member(m)) {
    constexpr auto fptr = default_impl<m, V, R, VoidSelf, impl_fptr_param_t<V, Args>...>();
    if constexpr(std::same_as<return_t, void>) {
        fptr(void_self, forward_v_table_fptr_param<V, Args>(args)...);
        return;
    } else {
        return fptr(void_self, forward_v_table_fptr_param<V, Args>(args)...);
    }
  } else {
    using VSelf = self_const_correct_t<V, VoidSelf>;
    if constexpr(std::same_as<return_t, void>) {
        [:m:](*static_cast<VSelf*>(void_self), forward_v_table_fptr_param<V, Args>(args)...);
        return;
    } else {
        return [:m:](*static_cast<VSelf*>(void_self), forward_v_table_fptr_param<V, Args>(args)...);
    }
  }
}

consteval std::meta::info make_vfimpl(std::meta::info concrete_type, std::meta::info implementing_function, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, std::meta::info interface_function) {
  std::vector<std::meta::info> types;
  bool use_default = annotations_of_with_type(implementing_function, ^^default_t).size() > 0;
  types.push_back(std::meta::reflect_constant(use_default));
  types.push_back(reflect_constant(implementing_function));
  types.push_back(std::meta::reflect_constant(dyn_self_val));
  types.push_back(std::meta::reflect_constant(dyn_self_cref));
  types.push_back(std::meta::reflect_constant(dyn_self_mutref));
  types.push_back(concrete_type);
  types.push_back(return_type_of(interface_function));
  add_v_table_fptr_this_param_type(interface_function, types);
  for (auto p : define_static_array(parameters_of(interface_function))) {
    types.push_back(make_v_table_fptr_param_type(types.size() == 7u, p));
  }
  return substitute(^^vfimpl, types);
}

template <typename VTable, std::meta::info Base, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref,
    typename Concrete, std::meta::info FunctionPointers>
consteval std::meta::info make_set_base_v_table_members();

template <typename VTable, std::meta::info dyn_self_val, std::meta::info dyn_self_cref, std::meta::info dyn_self_mutref, typename Concrete, std::meta::info FunctionPointers>
void set_v_table_members(VTable* v_table) {
    template for(constexpr auto v_table_spec : define_static_array(get_interface_specs(^^typename VTable::trait_declaration_t))){
        constexpr auto v_table_entry = anyxx26::meta::get_data_member_by_id(FunctionPointers, v_table_name_of(v_table_spec));
        if constexpr(is_v_table_data(v_table_spec)) {
            v_table->[:v_table_entry:] = [:v_table_spec.member:]::template init<Concrete>(v_table);
        } else {
            constexpr auto trait_template = template_of(v_table_spec.declaration_trait);
            constexpr auto args = std::define_static_array(template_arguments_of(v_table_spec.declaration_trait) | std::views::drop(2));
            constexpr auto implemenation_member = find_function_impl(v_table_spec.member, trait_template, ^^Concrete, args);
            v_table->[:v_table_entry:] = [:make_vfimpl(^^Concrete, implemenation_member, dyn_self_val, dyn_self_cref, dyn_self_mutref, v_table_spec.member):];
        }
    }
}

}  // namespace anyxx26
