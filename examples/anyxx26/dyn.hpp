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

template <template <typename, typename, typename...> typename Trait, typename... Args>
using dyn_self_val_t = dyn<Trait, anyxx::val<>, Args...>;

template <typename ...Ts>
struct select_last {
    using type = typename decltype((std::enable_if<true, Ts>{}, ...))::type;
};
template <typename ...Ts>
using proxy_from_args_t = typename select_last<Ts...>::type;

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

template<std::meta::info TraitTemplate, typename V, typename... Args>
consteval std::meta::info trait_model_map(){
    return substitute(TraitTemplate, {^^V, ^^model_map, ^^Args...});;
}
template<std::meta::info TraitTemplate, typename... Args>
consteval std::meta::info trait_declaration(){
    return substitute(TraitTemplate, {^^void*, ^^declaration, ^^Args...});;
}
template<template<typename, typename...> typename TraitTemplate, typename... Args>
using trait_declaration_t = TraitTemplate<void*, declaration, Args...>;

template <std::meta::info spec, typename V, typename R, typename VoidSelf,
          typename... Args>
R default_impl(VoidSelf voidSelf, Args&&... args) {
    using return_t = std::conditional_t<^^R == ^^anyxx::self, V, R>;
    constexpr auto ctx = std::meta::access_context::current();
    using self_t = self_const_correct_t<V, VoidSelf>;
    auto typed_self = static_cast<self_t*>(voidSelf);
    if constexpr(is_class_type(^^V)) {
        template for (constexpr auto candidate : define_static_array(members_of(^^V, ctx))) {
            if constexpr (!is_static_member(candidate) && is_function(candidate)) {
                if constexpr(has_identifier(candidate) && has_identifier(spec) && identifier_of(candidate) == identifier_of(spec)) {
                    if constexpr(std::is_invocable_r_v<return_t, decltype(&[:candidate:]), self_t, Args...>) {
                        return typed_self->[:candidate:](std::forward<Args>(args)...);
                    }
                }
                if constexpr(is_operator_function(candidate)) {
                    if constexpr(std::is_invocable_r_v<return_t, decltype(&[:candidate:]), self_t, Args...>) {
                        constexpr auto op = operator_of(candidate);
                        if constexpr(is_operator_function(spec)) {
                            if constexpr(op == operator_of(spec)) {
                                return typed_self->[:candidate:](std::forward<Args>(args)...);
                            }
                        } else if constexpr(anyxx26::meta::enum_to_string(op) == identifier_of(spec)) {
                            return typed_self->[:candidate:](std::forward<Args>(args)...);
                        }
                    }
                }
            }
        }
    } 
    if constexpr(meta::is_op_parentheses_spec<spec>()) {
        if constexpr(std::is_invocable_r_v<R, V, Args...>) {
            return (*typed_self)(std::forward<Args>(args)...);
        }
    }    
    throw std::logic_error(std::format("{} has no member function {}.", display_string_of(^^V), display_string_of(spec)));
}

template <typename R, typename... Args>
using v_table_fptr_type = R (*)(Args...);

template <std::meta::info p>
consteval std::meta::info make_v_table_fptr_param_type(bool self) {
  constexpr auto type = type_of(p);
  // constexpr auto name = identifier_of(p);
  if (self) {
    if constexpr (type == ^^void* const& || type == ^^void const* ||
                  type == ^^void const*&) {
      return ^^void const*;
    } else if constexpr (type == ^^void*& || type == ^^void*) {
      return ^^void*;
    } else if (is_const(remove_reference(type))) {
      return ^^void const*;
    } else {
      return ^^void*;
    }
  } else {
    return type;
  }
}

template <std::meta::info f>
consteval void add_v_table_fptr_this_param_type(std::vector<std::meta::info>& types) {
    if(!is_static_member(f)) {
        if(is_const(f)) {
            types.push_back(^^void const*);
        } else {
            types.push_back(^^void*);
        }
    }
}

template <std::meta::info f, std::meta::info dyn_self_val>
consteval void add_v_table_fptr_return_type(std::vector<std::meta::info>& types) {
    constexpr auto return_spec = return_type_of(f);
    if(return_spec == ^^anyxx::self) {
        types.push_back(dyn_self_val);
    } else {
        types.push_back(return_spec);
    }
}

template <std::meta::info f, std::meta::info dyn_self_val>
consteval std::meta::info make_v_table_fptr_type() {
  std::vector<std::meta::info> types;
  add_v_table_fptr_return_type<f, dyn_self_val>(types);
  add_v_table_fptr_this_param_type<f>(types);
  template for (constexpr auto p : define_static_array(parameters_of(f))) {
    types.push_back(make_v_table_fptr_param_type<p>(types.size() == 1));
  }
  return substitute(^^v_table_fptr_type, types);
}

template <std::meta::info TraitDeclaration, std::meta::info dyn_self_val>
consteval void collect_v_table_members(std::vector<std::meta::info>& fptrs) {
    constexpr auto base = meta::get_single_public_base<TraitDeclaration>();
    if constexpr(base != std::meta::info{}) {
        collect_v_table_members<type_of(base), dyn_self_val>(fptrs);
    }

    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m :
        define_static_array(members_of(TraitDeclaration, ctx))) {
        if constexpr(has_identifier(m) && is_type(m) && annotations_of_with_type(m, ^^v_table_data_t).size() > 0) {
            using type = [:m:]::type;
            auto dms = std::meta::data_member_spec(dealias(^^type), { .name = identifier_of(m) });
            fptrs.push_back(reflect_constant(dms));
        }
        else if constexpr(has_identifier(m) && is_function(m)) {
            auto ft = make_v_table_fptr_type<m, dyn_self_val>();
            auto dms = std::meta::data_member_spec(
                ft, { .name = identifier_of(m) });
            fptrs.push_back(reflect_constant(dms));
        }
        else if constexpr(is_user_declared(m) && is_operator_function(m)) {
            auto ft = make_v_table_fptr_type<m, dyn_self_val>();
            auto dms = std::meta::data_member_spec(
                ft, { .name = anyxx26::meta::enum_to_string(operator_of(m)) });
            fptrs.push_back(reflect_constant(dms));
        }
    }
};

template <template <typename, typename, typename...> typename Trait, typename... Args>
consteval std::meta::info make_v_table_members_type() {
    std::vector<std::meta::info> fptrs;
    collect_v_table_members<^^Trait<void*, declaration, Args...>, ^^dyn_self_val_t<Trait, Args...>>(fptrs);
    return substitute(^^meta::to_struct, fptrs);
}

template <bool default_, std::meta::info m, std::meta::info dyn_self_val, typename V, typename R,
          typename VoidSelf, typename... Args>
std::conditional_t<^^R == ^^anyxx::self, typename [:dyn_self_val:], R> vfimpl(VoidSelf self, Args... args) {
  if constexpr (default_ || !is_static_member(m)) {
    return default_impl<m, V, R>(self, std::forward<Args>(args)...);
  } else {
    using VSelf = self_const_correct_t<V, VoidSelf>;
    return std::forward<R>([:m:](*static_cast<VSelf*>(self), std::forward<Args>(args)...));
  }
}

template <typename V, std::meta::info f, std::meta::info dyn_self_val>
consteval std::meta::info make_vfimpl() {
  std::vector<std::meta::info> types;
  bool use_default = annotations_of_with_type(f, ^^default_t).size() > 0;
  types.push_back(std::meta::reflect_constant(use_default));
  types.push_back(reflect_constant(f));
  types.push_back(std::meta::reflect_constant(dyn_self_val));
  types.push_back(^^V);
  add_v_table_fptr_return_type<f, dyn_self_val>(types);
  add_v_table_fptr_this_param_type<f>(types);
  template for (constexpr auto p : define_static_array(parameters_of(f))) {
    types.push_back(make_v_table_fptr_param_type<p>(types.size() == 5u));
  }
  return substitute(^^vfimpl, types);
}

template <template <typename, typename, typename...> typename Trait>
using base_v_table_t = anyxx::observeable::v_table_t;

template <std::meta::info Trait, std::meta::info interface_function>
consteval std::optional<std::meta::info> find_function_impl() {
    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(Trait, ctx))) {
        if constexpr(interface_function == m){
            return { m };
        }
        if constexpr(is_function(m) && has_identifier(m))
        {
            if constexpr(is_function(interface_function) && has_identifier(interface_function)) {
                if constexpr(identifier_of(interface_function) == identifier_of(m)) {
                    return { m };
                }
            }
            else if constexpr(is_operator_function(interface_function)) {
                if constexpr(anyxx26::meta::enum_to_string(operator_of(interface_function)) == identifier_of(m)) {
                    return { m };
                }
            }
        }
    }
    return {};
}

template <std::meta::info TraitTemplate, typename V, std::meta::info InterfaceFunction, typename... Args>
consteval std::meta::info find_function_impl() {

  constexpr auto found_in_impl = find_function_impl<trait_model_map<TraitTemplate, V, Args...>(), InterfaceFunction >();
  if constexpr (found_in_impl) {
    return *found_in_impl;
  } 
  constexpr auto found_in_base = find_function_impl<trait_declaration<TraitTemplate, Args...>(), InterfaceFunction >();
  if constexpr(found_in_base) {
      return *found_in_base;
  }
  throw std::logic_error("Function not found in impl trait or base trait");
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct v_table;


template <std::meta::info Trait, std::meta::info dyn_self_val, typename Concrete, std::meta::info FunctionPointers, typename... Args>
void set_v_table_members(auto* v_table) {
    constexpr auto ctx = std::meta::access_context::current();

    constexpr auto td = trait_declaration<Trait, Args...>();
    constexpr auto base = meta::get_single_public_base<td>();
    if constexpr(base != std::meta::info{}) {
        if constexpr(has_template_arguments(type_of(base)) && template_arguments_of(type_of(base)).size() > 2u) {
            //constexpr auto base_trait_template = template_of(type_of(base));
            //auto call_params = std::vector{ reflect_constant(base_trait_template),^^ Concrete, reflect_constant(FunctionPointers) };
            //constexpr auto base_trait_params = template_arguments_of(base_trait_template) | std::views::drop(2); // self, trait-specifier
            //call_params.append_range(base_trait_params);
            //consteval{ auto call_set_v_table_fptrs_with_base_trait_args = substitute(^^set_v_table_members, call_params); }
            //[:call_set_v_table_fptrs_with_base_trait_args:] (v_table);
        } else {
            set_v_table_members<template_of(type_of(base)), dyn_self_val, Concrete, FunctionPointers>(v_table);
        }
    }

    template for(constexpr auto interface_m : define_static_array(members_of(trait_declaration<Trait, Args...>(), ctx))) {
        if constexpr(has_identifier(interface_m) && is_type(interface_m) && annotations_of_with_type(interface_m, ^^ v_table_data_t).size() > 0) {
            constexpr auto m = anyxx26::meta::get_member<FunctionPointers, interface_m>();
            v_table->[:m:] = [:interface_m:]::template init<Concrete>(v_table);
        }
        if constexpr((has_identifier(interface_m) && is_function(interface_m))
            || (is_user_declared(interface_m) && is_operator_function(interface_m))) {
            constexpr auto f = anyxx26::meta::get_member<FunctionPointers, interface_m>();
            constexpr auto m = find_function_impl<Trait, Concrete, interface_m, Args...>();
            v_table->[:f:] = [:make_vfimpl<Concrete, m, dyn_self_val>():];
        }
    }
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct v_table
    : base_v_table_t<Trait>,
    [: make_v_table_members_type<Trait, Args...>() :] {
    using v_table_t = v_table;
	using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
    using  fptrs_t = [:make_v_table_members_type<Trait, Args...>():];
    template <typename Concrete>
    v_table(std::in_place_type_t<Concrete> concrete)
        : base_v_table_t<Trait>(concrete) {
        set_v_table_members<^^Trait, ^^dyn_self_val_t<Trait, Args...>, Concrete, ^^fptrs_t, Args...>(this);
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

template <template <typename, typename, typename...> typename Trait, typename Proxy, typename... Args >
struct dyn_base {
  using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
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
      : dyn_base(std::move(other.proxy_), other.release_v_table()) {}
  dyn_base& operator=(dyn_base&& other) noexcept
    requires(anyxx::moveable_from<proxy_t, proxy_t>)
  {
    proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_), other.v_table_);
    v_table_ = other.release_v_table();
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
      : dyn_base(std::move(other.proxy_), v_table_cast<v_table_t>(other.release_v_table())) {
  }
  template <is_dyn Other>
  dyn_base& operator=(Other&& other) noexcept
      requires(anyxx::moveable_from<proxy_t, typename Other::proxy_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
  {
      proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_), other.v_table_);
      v_table_ = v_table_cast<v_table_t>(other.release_v_table());
      return *this;
  }

  template <typename Self, typename... Params>
  decltype(auto) operator()(this Self&& self, Params&&... params) {
      auto x = anyxx::get_proxy_ptr(self.proxy_, self.v_table_);
      return self.v_table_->op_parentheses(x, std::forward<Params>(params)...);
  }

#define __dyn_OP(function, op) \
  template <typename Self, typename... Params> \
  decltype(auto) operator op (this Self&& self, Params&&... params) { \
      auto x = anyxx::get_proxy_ptr(self.proxy_, self.v_table_); \
      return self.v_table_->function(x, std::forward<Params>(params)...); \
  }
#define __dyn_OP0(function, op) \
  template <typename Self> \
  decltype(auto) operator op (this Self&& self) { \
      auto x = anyxx::get_proxy_ptr(self.proxy_, self.v_table_); \
      return self.v_table_->function(x); \
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
    __dyn_OP(op_plus_plus, ++)
    __dyn_OP(op_minus_minus, --)

#undef __dyn_OP
#undef __dyn_OP0

  template <typename... Params>
  decltype(auto) operator++(int) {
    auto old = *this
    ++(*this);
    return old;
  }
  template <typename... Params>
  decltype(auto) operator--(int) {
    auto old = *this
    --(*this);
    return old;
  }

  auto release_v_table() { return std::exchange(v_table_, nullptr); }
};

template <typename DynBase, std::meta::info f>
struct dyn_facade_call {
  template <typename... Args>
  auto operator()(Args&&... args) const {
    auto base = reinterpret_cast<DynBase const*>(this);
    using v_table_t = DynBase::v_table_t;
    auto v_table_ptr = base->v_table_;
    using fptrs_t = typename v_table_t::fptrs_t;
    auto fptrs = static_cast<fptrs_t*>(v_table_ptr);
    auto constexpr vf = anyxx26::meta::get_member<^^fptrs_t, f>();
    auto x = anyxx::get_proxy_ptr(base->proxy_, v_table_ptr);
    return fptrs->[:vf:](x, std::forward<Args>(args)...);
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
    constexpr auto base = meta::get_single_public_base<TraitDeclaration>();
    if constexpr(base != std::meta::info{}) {
        collect_dyn_facade_calls<type_of(base), DynBase>(calls);
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
  collect_dyn_facade_calls<trait_declaration<^^Trait, Args...>(), dyn_base<Trait, Proxy, Args...>>(calls);
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
