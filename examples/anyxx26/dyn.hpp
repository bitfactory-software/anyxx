#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <examples/anyxx26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>

#pragma GCC diagnostic ignored "-Wunused-function"

namespace anyxx26 {

template <typename V, typename VoidSelf>
using self_const_correct_t = std::conditional_t<
    std::is_const_v<std::remove_pointer_t<std::remove_reference_t<VoidSelf>>>,
    V const, V>;

struct default_t {};
constexpr static inline default_t defaulted = {};

struct declaration {};
struct model_map {};

template <std::meta::info m, typename V, typename R, typename VoidSelf,
          typename... Args>
R default_impl(VoidSelf self, Args&&... args) {
  constexpr auto ctx = std::meta::access_context::current();
  template for (constexpr auto mc : define_static_array(members_of(^^V, ctx))) {
    if constexpr (has_identifier(mc) && !is_static_member(mc) &&
                  is_function(mc) && identifier_of(mc) == identifier_of(m)) {
      using self_t = self_const_correct_t<V, VoidSelf>;
      auto self_ptr = static_cast<self_t*>(self);
      static_assert(
          std::is_invocable_r_v<R, decltype(&[:mc:]), self_t*, Args...>);
      return self_ptr->[:mc:](std::forward<Args>(args)...);
    }
  }
}

template <typename R, typename... Args>
using v_table_fptr_type = R (*)(Args...);

template <std::meta::info p>
consteval std::meta::info make_v_table_fptr_param_type(int i) {
  constexpr auto type = type_of(p);
  // constexpr auto name = identifier_of(p);
  if (i == 1) {
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
consteval std::meta::info make_v_table_fptr_type() {
  std::vector<std::meta::info> types;
  types.push_back(return_type_of(f));
  auto i = 0;
  template for (constexpr auto p : define_static_array(parameters_of(f))) {
    ++i;
    types.push_back(make_v_table_fptr_param_type<p>(i));
  }
  return substitute(^^v_table_fptr_type, types);
}

template <std::meta::info TraitDeclaration>
consteval void collect_v_table_fs(std::vector<std::meta::info>& fptrs) {
    constexpr auto base = meta::get_single_public_base<TraitDeclaration>();
    if constexpr(base != std::meta::info{}) {
        collect_v_table_fs<base>(fptrs);
    }

    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m :
        define_static_array(members_of(TraitDeclaration, ctx))) {
        if constexpr(has_identifier(m) && is_static_member(m) && is_function(m)) {
            auto ft = make_v_table_fptr_type<m>();
            auto dms = std::meta::data_member_spec(
                ft, { .name = identifier_of(m), .no_unique_address = true });
            fptrs.push_back(reflect_constant(dms));
        }
    }
};

template <template <typename, typename> typename Trait>
consteval std::meta::info make_v_table_fptrs_type() {
    std::vector<std::meta::info> fptrs;
    collect_v_table_fs<^^Trait<void*, declaration>>(fptrs);
    return substitute(^^meta::to_struct, fptrs);
}

template <bool default_, std::meta::info m, typename V, typename R,
          typename VoidSelf, typename... Args>
R vfimpl(VoidSelf self, Args... args) {
  if constexpr (default_) {
    return default_impl<m, V, R>(self, std::forward<Args>(args)...);
  } else {
    using VSelf = self_const_correct_t<V, VoidSelf>;
    return [:m:](*static_cast<VSelf*>(self), std::forward<Args>(args)...);
  }
}

template <typename V, std::meta::info f>
consteval std::meta::info make_vfimpl() {
  std::vector<std::meta::info> types;
  bool use_default = annotations_of_with_type(f, ^^default_t).size() > 0;
  types.push_back(std::meta::reflect_constant(use_default));
  types.push_back(reflect_constant(f));
  types.push_back(^^V);
  types.push_back(return_type_of(f));
  auto i = 0;
  template for (constexpr auto p : define_static_array(parameters_of(f))) {
    ++i;
    types.push_back(make_v_table_fptr_param_type<p>(i));
  }
  return substitute(^^vfimpl, types);
}

 //template <template <typename, typename> typename Trait>
 //using base_v_table_t =
 //    typename[:anyxx26::meta::get_first_public_base<
 //                  ^^Trait<void*, declaration>, ^^anyxx::observeable>():] ::v_table_t;

template <template <typename, typename> typename Trait>
using base_v_table_t = anyxx::observeable::v_table_t;

template <std::meta::info Trait, std::meta::info interface_function>
consteval std::optional<std::meta::info> find_function_impl() {

    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(Trait, ctx))) {
        if constexpr(has_identifier(m) && is_static_member(m) && is_function(m)
            && identifier_of(interface_function) == identifier_of(m)) {
            return { m };
        }
    }
    return {};
}

template <std::meta::info TraitTemplate, typename V, std::meta::info InterfaceFunction>
consteval std::meta::info find_function_impl() {

  constexpr auto found_in_impl = find_function_impl<substitute(TraitTemplate, {^^V, ^^model_map}), InterfaceFunction >();
  if constexpr (found_in_impl) {
    return *found_in_impl;
  } 
  constexpr auto found_in_base = find_function_impl<substitute(TraitTemplate, {^^V, ^^declaration}), InterfaceFunction >();
  if constexpr(found_in_base) {
      return *found_in_base;
  }
  throw std::logic_error("Function not found in impl trait or base trait");
}

template <template <typename, typename> typename Trait>
struct v_table;

template <std::meta::info Trait, typename Concrete, std::meta::info FunctionPointers>
void set_v_table_fptrs(auto* v_table) {
    constexpr auto ctx = std::meta::access_context::current();

	constexpr auto trait_declaration = substitute(Trait, {^^void*, ^^declaration});

    constexpr auto base = meta::get_single_public_base<trait_declaration>();
    if constexpr(base != std::meta::info{}) {
        set_v_table_fptrs<template_of(base), Concrete, FunctionPointers>(v_table);
    }

    template for(constexpr auto interface_m : define_static_array(members_of(trait_declaration, ctx))) {
        if constexpr(has_identifier(interface_m) &&
            is_static_member(interface_m) && is_function(interface_m)) {
            constexpr auto f =
                anyxx26::meta::get_member<FunctionPointers, interface_m>();
            constexpr auto m = find_function_impl<Trait, Concrete, interface_m>();
            v_table->[:f:] = [:make_vfimpl<Concrete, m>():];
        }
    }
}

template <template <typename, typename> typename Trait>
struct v_table
    : base_v_table_t<Trait>,
    [: make_v_table_fptrs_type<Trait>() :] {
    using v_table_t = v_table;
    using  fptrs = [:make_v_table_fptrs_type<Trait>():];
    template <typename Concrete>
    v_table(std::in_place_type_t<Concrete> concrete)
        : base_v_table_t<Trait>(concrete) {
        set_v_table_fptrs<^^Trait, Concrete, ^^fptrs>(this);
    }
};

template <template <typename, typename> typename Trait, typename V>
v_table<Trait>* get_v_table_instance() {
  static v_table<Trait> instance(std::in_place_type<V>);
  return &instance;
};

template <template <typename, typename> typename Trait, anyxx::is_proxy Proxy>
struct dyn_base {
  using proxy_t = Proxy;
  using proxy_trait_t = anyxx::proxy_trait<proxy_t>;
  using void_t = typename proxy_trait_t::void_t;
  using v_table_t = v_table<Trait>;

  v_table_t* v_table_;
  Proxy proxy_{};

  dyn_base()
    requires proxy_trait_t::allow_any_default_constructibile
  {}

  template <typename ConstructedWith>
  explicit(false) dyn_base(ConstructedWith&& constructed_with)  // NOLINT
    requires anyxx::constructibile_for<ConstructedWith, proxy_t,
                                       dyn_base<Trait, Proxy>>
      : v_table_(get_v_table_instance<Trait, std::decay_t<ConstructedWith>>()),
        proxy_(anyxx::erased<proxy_t>(
            std::forward<ConstructedWith>(constructed_with))) {}

  template <typename V>
    requires(!anyxx::is_lifetime_bound<Proxy>)
  dyn_base(std::in_place_t, V&& v)
      : v_table_(get_v_table_instance<Trait, V>()),
        proxy_(proxy_trait_t::construct_in_place(std::forward<V>(v))) {}

  template <typename T, typename... Args>
    requires(!anyxx::is_lifetime_bound<Proxy>)
  dyn_base(std::in_place_type_t<T>, Args&&... args)
      : v_table_(get_v_table_instance<Trait, T>()),
        proxy_(proxy_trait_t::template construct_type_in_place<T>(
            std::forward<Args>(args)...)) {}

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
    proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_),
                           other.v_table_);
    v_table_ = other.release_v_table();
    return *this;
  }

 private:
  auto release_v_table() { return std::exchange(v_table_, nullptr); }
};

template <template <typename, typename> typename Trait, typename Proxy,
          std::meta::info vf>
struct dyn_facade_call {
  template <typename... Args>
  auto operator()(Args&&... args) const {
    auto base = reinterpret_cast<dyn_base<Trait, Proxy> const*>(this);
    auto v_table_ptr = base->v_table_;
    auto fptrs = static_cast<typename v_table<Trait>::fptrs*>(v_table_ptr);
    auto x = anyxx::get_proxy_ptr(base->proxy_, v_table_ptr);
    return fptrs->[:vf:](x, std::forward<Args>(args)...);
  }
};

template <template <typename, typename> typename Trait, typename Proxy>
consteval std::meta::info make_dyn_facade() {
  constexpr auto v_table_t_info = make_v_table_fptrs_type<Trait>();

  std::vector<std::meta::info> calls;
  constexpr auto ctx = std::meta::access_context::current();
  template for (constexpr auto m :
                define_static_array(members_of(v_table_t_info, ctx))) {
    if constexpr (has_identifier(m)) {
      using dyn_facade_call_t = dyn_facade_call<Trait, Proxy, m>;
      constexpr std::meta::info call_meta = ^^dyn_facade_call_t;
      auto dms = std::meta::data_member_spec(
          call_meta, {.name = identifier_of(m), .no_unique_address = true});
      calls.push_back(reflect_constant(dms));
    }
  }
  return substitute(^^meta::to_struct, calls);
};

template <template <typename, typename> typename Trait, typename Proxy>
struct dyn : dyn_base<Trait, Proxy>, [:make_dyn_facade<Trait, Proxy>():] {
  using dyn_base<Trait, Proxy>::dyn_base;
};

}  // namespace anyxx26
