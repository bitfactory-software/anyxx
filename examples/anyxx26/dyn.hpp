#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <examples/anyxx26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>

namespace anyxx26 {

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

template<std::meta::info TraitTemplate, typename V>
consteval std::meta::info trait_model_map(){
    return substitute(TraitTemplate, {^^V, ^^model_map});;
}
template<std::meta::info TraitTemplate>
consteval std::meta::info trait_declaration(){
    return substitute(TraitTemplate, {^^void*, ^^declaration });;
}
template<template<typename, typename, typename...> typename TraitTemplate>
using trait_declaration_t = TraitTemplate<void*, declaration>;

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

template <std::meta::info f>
consteval std::meta::info make_v_table_fptr_type() {
  std::vector<std::meta::info> types;
  types.push_back(return_type_of(f));
  add_v_table_fptr_this_param_type<f>(types);
  template for (constexpr auto p : define_static_array(parameters_of(f))) {
    types.push_back(make_v_table_fptr_param_type<p>(types.size() == 1));
  }
  return substitute(^^v_table_fptr_type, types);
}

template <std::meta::info TraitDeclaration>
consteval void collect_v_table_fs(std::vector<std::meta::info>& fptrs) {
    constexpr auto base = meta::get_single_public_base<TraitDeclaration>();
    if constexpr(base != std::meta::info{}) {
        collect_v_table_fs<type_of(base)>(fptrs);
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
            auto ft = make_v_table_fptr_type<m>();
            auto dms = std::meta::data_member_spec(
                ft, { .name = identifier_of(m) });
            fptrs.push_back(reflect_constant(dms));
        }
    }
};

template <template <typename, typename, typename...> typename Trait>
consteval std::meta::info make_v_table_fptrs_type() {
    std::vector<std::meta::info> fptrs;
    collect_v_table_fs<^^Trait<void*, declaration>>(fptrs);
    return substitute(^^meta::to_struct, fptrs);
}

template <bool default_, std::meta::info m, typename V, typename R,
          typename VoidSelf, typename... Args>
R vfimpl(VoidSelf self, Args... args) {
  if constexpr (default_ || !is_static_member(m)) {
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
  add_v_table_fptr_this_param_type<f>(types);
  template for (constexpr auto p : define_static_array(parameters_of(f))) {
    types.push_back(make_v_table_fptr_param_type<p>(types.size() == 4u));
  }
  return substitute(^^vfimpl, types);
}

 //template <template <typename, typename> typename Trait>
 //using base_v_table_t =
 //    typename[:anyxx26::meta::get_first_public_base<
 //                  ^^Trait<void*, declaration>, ^^anyxx::observeable>():] ::v_table_t;

template <template <typename, typename, typename...> typename Trait>
using base_v_table_t = anyxx::observeable::v_table_t;

template <std::meta::info Trait, std::meta::info interface_function>
consteval std::optional<std::meta::info> find_function_impl() {

    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m : define_static_array(members_of(Trait, ctx))) {
        if constexpr(has_identifier(m) && is_function(m)
            && identifier_of(interface_function) == identifier_of(m)) {
            return { m };
        }
    }
    return {};
}

template <std::meta::info TraitTemplate, typename V, std::meta::info InterfaceFunction>
consteval std::meta::info find_function_impl() {

  constexpr auto found_in_impl = find_function_impl<trait_model_map<TraitTemplate, V>(), InterfaceFunction >();
  if constexpr (found_in_impl) {
    return *found_in_impl;
  } 
  constexpr auto found_in_base = find_function_impl<trait_declaration<TraitTemplate>(), InterfaceFunction >();
  if constexpr(found_in_base) {
      return *found_in_base;
  }
  throw std::logic_error("Function not found in impl trait or base trait");
}

template <template <typename, typename, typename...> typename Trait>
struct v_table;

template <std::meta::info Trait, typename Concrete, std::meta::info FunctionPointers>
void set_v_table_fptrs(auto* v_table) {
    constexpr auto ctx = std::meta::access_context::current();

    constexpr auto base = meta::get_single_public_base<trait_declaration<Trait>()>();
    if constexpr(base != std::meta::info{}) {
        set_v_table_fptrs<template_of(type_of(base)), Concrete, FunctionPointers>(v_table);
    }

    template for(constexpr auto interface_m : define_static_array(members_of(trait_declaration<Trait>(), ctx))) {
        if constexpr(has_identifier(interface_m) && is_type(interface_m) && annotations_of_with_type(interface_m, ^^ v_table_data_t).size() > 0) {
            constexpr auto m = anyxx26::meta::get_member<FunctionPointers, interface_m>();
            v_table->[:m:] = [:interface_m:]::template init<Concrete>(v_table);
        }
        if constexpr(has_identifier(interface_m) && is_function(interface_m)) {
            constexpr auto f = anyxx26::meta::get_member<FunctionPointers, interface_m>();
            constexpr auto m = find_function_impl<Trait, Concrete, interface_m>();
            v_table->[:f:] = [:make_vfimpl<Concrete, m>():];
        }
    }
}

template <template <typename, typename, typename...> typename Trait>
struct v_table
    : base_v_table_t<Trait>,
    [: make_v_table_fptrs_type<Trait>() :] {
    using v_table_t = v_table;
	using trait_declaration_t = anyxx26::trait_declaration_t<Trait>;
    using  fptrs_t = [:make_v_table_fptrs_type<Trait>():];
    template <typename Concrete>
    v_table(std::in_place_type_t<Concrete> concrete)
        : base_v_table_t<Trait>(concrete) {
        set_v_table_fptrs<^^Trait, Concrete, ^^fptrs_t>(this);
    }
};

template <template <typename, typename, typename...> typename Trait, typename V>
v_table<Trait>* get_v_table_instance() {
  static v_table<Trait> instance(std::in_place_type<V>);
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

template <template <typename, typename, typename...> typename Trait, anyxx::is_proxy Proxy>
struct dyn_base {
  using trait_declaration_t = anyxx26::trait_declaration_t<Trait>;
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
    proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_), other.v_table_);
    v_table_ = other.release_v_table();
    return *this;
  }

  template <is_dyn Other>
  explicit(false) dyn_base(const Other& other)  // NOLINT(noExplicitConstructor)
      requires(anyxx::proxy_borrowable_from<proxy_t, typename Other::proxy_t, typename Other::v_table_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
      : v_table_(v_table_cast<v_table_t>(other.v_table_)),
      proxy_(borrow_proxy_as<Proxy>(other.proxy_, other.v_table_)) {
  }
  template <is_dyn Other>
  dyn_base& operator=(Other const& other)
      requires(anyxx::proxy_borrowable_from<proxy_t, typename Other::proxy_t, typename Other::v_table_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
  {
      v_table_ = v_table_cast<v_table_t>(other.v_table_);
      proxy_ = anyxx::borrow_proxy_as<Proxy>(other.proxy_, other.v_table_);
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

template <std::meta::info TraitDeclaration, typename DynBase>
consteval void collect_dyn_facade_calls(std::vector<std::meta::info>& calls) {
    constexpr auto base = meta::get_single_public_base<TraitDeclaration>();
    if constexpr(base != std::meta::info{}) {
        collect_dyn_facade_calls<type_of(base), DynBase>(calls);
    }

    constexpr auto ctx = std::meta::access_context::current();
    template for(constexpr auto m :
        define_static_array(members_of(TraitDeclaration, ctx))) {
        if constexpr(has_identifier(m) && is_function(m)) {
            using dyn_facade_call_t = dyn_facade_call<DynBase, m>;
            constexpr std::meta::info call_meta = ^^dyn_facade_call_t;
            auto dms = std::meta::data_member_spec(
                call_meta, { .name = identifier_of(m), .no_unique_address = true });
            calls.push_back(reflect_constant(dms));
        }
    }
};

template <template <typename, typename, typename...> typename Trait, typename Proxy>
consteval std::meta::info make_dyn_facade() {

  std::vector<std::meta::info> calls;
  collect_dyn_facade_calls<trait_declaration<^^Trait>(), dyn_base<Trait, Proxy>>(calls);
  return substitute(^^meta::to_struct, calls);
};

template <template <typename, typename, typename...> typename Trait, typename Proxy>
struct dyn : dyn_base<Trait, Proxy>, [:make_dyn_facade<Trait, Proxy>():] {
  using dyn_base<Trait, Proxy>::dyn_base;
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
