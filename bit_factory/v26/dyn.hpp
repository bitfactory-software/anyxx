#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/dyn/facade.hpp>
#include <bit_factory/v26/dyn/keywords.hpp>
#include <bit_factory/v26/dyn/signature_translation.hpp>
#include <bit_factory/v26/dyn/set_v_table_members.hpp>
#include <bit_factory/v26/dyn/make_v_table_members_type.hpp>
#include <bit_factory/v26/meta/utilities.hpp>
#include <meta>
#include <utility>
#include <vector>

namespace anyxx26 {

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct v_table
    : base_v_table_t<Trait>,
    [: make_v_table_members_type<Trait, Args...>():] {
    using v_table_t = v_table;
	using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
    using fptrs_t = [:make_v_table_members_type<Trait, Args...>():];
    template <typename Concrete>
    v_table(std::in_place_type_t<Concrete> concrete)
        : base_v_table_t<Trait>(concrete) {
        set_v_table_members<v_table_t, ^^dyn_self_val_t<Trait, Args...>, ^^dyn_self_cref_t<Trait, Args...>, ^^dyn_self_mutref_t<Trait, Args...>, Concrete, ^^fptrs_t>(this);
    }
};

template <template <typename, typename, typename...> typename Trait, typename V, typename... Args>
v_table<Trait, Args...>* v_table_instance() {
   return anyxx::v_table_instance<v_table<Trait, Args...>, V>();
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


template <template <typename, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct dyn_base : deduced_typenames<Trait, Args...> {
  using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
  using dyn_self_t = dyn<Trait, Args...>;
  using dyn_self_cref_t = dyn<Trait, anyxx::cref, Args...>;
  using dyn_self_mutref_t = dyn<Trait, anyxx::mutref, Args...>;
  using proxy_t = Proxy;
  using proxy_trait_t = anyxx::proxy_trait<proxy_t>;
  using void_t = typename proxy_trait_t::void_t;
  using v_table_t = v_table<Trait, Args...>;
  inline static constexpr bool is_dyn = true; 

  v_table_t* v_table_;
  proxy_t proxy_{};

  dyn_base()
    requires proxy_trait_t::allow_any_default_constructibile
  {}

  template <typename ConstructedWith>
  explicit(false) dyn_base(ConstructedWith&& constructed_with)  // NOLINT
    requires anyxx::constructibile_for<ConstructedWith, proxy_t,
                                       dyn_base<Trait, proxy_t, Args...>>
      : v_table_(v_table_instance<Trait, std::decay_t<ConstructedWith>, Args...>()),
        proxy_(anyxx::erased<proxy_t>(
            std::forward<ConstructedWith>(constructed_with))) {}

  template <typename V>
    requires(!anyxx::is_lifetime_bound<proxy_t>)
  dyn_base(std::in_place_t, V&& v)
      : v_table_(v_table_instance<Trait, V, Args...>()),
        proxy_(proxy_trait_t::construct_in_place(std::forward<V>(v))) {}

  template <typename T, typename... ConstructWithArgs>
    requires(!anyxx::is_lifetime_bound<proxy_t>)
  dyn_base(std::in_place_type_t<T>, ConstructWithArgs&&... args)
      : v_table_(v_table_instance<Trait, T, Args...>()),
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

  template <anyxx26::is_dyn Other>
  explicit(false) dyn_base(const Other& other)  // NOLINT(noExplicitConstructor)
      requires(anyxx::proxy_borrowable_from<proxy_t, typename Other::proxy_t, typename Other::v_table_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
      : v_table_(v_table_cast<v_table_t>(other.v_table_)),
      proxy_(borrow_proxy_as<proxy_t>(other.proxy_, other.v_table_)) {
  }
  template <anyxx26::is_dyn Other>
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
  template <anyxx26::is_dyn Other>
  explicit(false) dyn_base(Other&& other) noexcept  // NOLINT(noExplicitConstructor)
      requires(anyxx::moveable_from<proxy_t, typename Other::proxy_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
      : dyn_base(std::move(other.proxy_), v_table_cast<v_table_t>(release_v_table(other))) {
  }
  template <anyxx26::is_dyn Other>
  dyn_base& operator=(Other&& other) noexcept
      requires(anyxx::moveable_from<proxy_t, typename Other::proxy_t> &&
        std::derived_from<typename Other::trait_declaration_t, trait_declaration_t>)
  {
      proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_), other.v_table_);
      v_table_ = v_table_cast<v_table_t>(release_v_table(other));
      return *this;
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

    __dyn_OP(op_parentheses, ())
    __dyn_OP(op_square_brackets, [])
    __dyn_OP0(op_arrow, ->)
    __dyn_OP0(op_plus_plus, ++)
    __dyn_OP0(op_minus_minus, --)

//    __dyn_OP(op_ampersand, &)

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

template <template <typename, typename, typename...> typename Trait, typename... Args>
struct dyn : dyn_base<Trait, Args...>, [:make_dyn_facade<Trait, Args...>():] {
  using dyn_base<Trait, Args...>::dyn_base;
};
template <template <typename, typename, typename...> typename Trait>
struct dyn<Trait> : dyn_base<Trait, default_proxy_t<Trait>>, [:make_dyn_facade<Trait, default_proxy_t<Trait>>():] {
    using dyn_base<Trait, default_proxy_t<Trait>>::dyn_base;
};
template <template <typename, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct dyn<Trait, Proxy, Args...> : dyn_base<Trait, Proxy, Args...>, [:make_dyn_facade<Trait, Proxy, Args...>():] {
    using dyn_base<Trait, Proxy, Args...>::dyn_base;
};
template <template <typename, typename, typename...> typename Trait, typename Arg0, typename... Args>
struct dyn<Trait, Arg0, Args...> : dyn_base<Trait, default_proxy_t<Trait, Arg0, Args...>, Arg0, Args...>, 
    [:make_dyn_facade<Trait, default_proxy_t<Trait, Arg0, Args...>, Arg0, Args...>():] {
    using dyn_base<Trait, default_proxy_t<Trait, Arg0, Args...>, Arg0, Args...>::dyn_base;
};

#define __dyn_OP_CONST(function, op) \
template <template <typename, typename, typename...> typename Trait, typename Other, typename... Args> \
decltype(auto) operator op (dyn<Trait, Args...> const& lhs, Other const& rhs) { \
    return lhs.function(rhs); \
}
#define __dyn_OP_MUTATING(function, op) \
template <template <typename, typename, typename...> typename Trait, typename Other, typename... Args> \
decltype(auto) operator op (dyn<Trait, Args...>& lhs, Other const& rhs) { \
    return lhs.function(rhs); \
}
#define __dyn_OP0(function, op) \
template <template <typename, typename, typename...> typename Trait, typename... Args> \
decltype(auto) operator op (dyn<Trait, Args...> const& lhs) { \
    return lhs.function(); \
}
#define __dyn_OP0_MUTATING(function, op) \
template <template <typename, typename, typename...> typename Trait, typename... Args> \
decltype(auto) operator op (dyn<Trait, Args...>& lhs) { \
    return lhs.function(); \
}

__dyn_OP_CONST(op_equals_equals, ==)
__dyn_OP_CONST(op_exclamation_equals, != )
__dyn_OP_CONST(op_less, < )
__dyn_OP_CONST(op_greater, > )
__dyn_OP_CONST(op_less_equals, <= )
__dyn_OP_CONST(op_greater_equals, >= )
__dyn_OP_CONST(op_spaceship, <=> )

__dyn_OP0(op_tilde, ~)
__dyn_OP0(op_exclamation, !)
__dyn_OP0(op_plus, +)
__dyn_OP0(op_minus, -)
__dyn_OP0(op_star, *)
__dyn_OP0_MUTATING(op_star, *)

__dyn_OP_CONST(op_plus, +)
__dyn_OP_CONST(op_minus, -)
__dyn_OP_CONST(op_star, *)
__dyn_OP_CONST(op_slash, / )
__dyn_OP_CONST(op_percent, %)
__dyn_OP_CONST(op_caret, ^)
__dyn_OP_CONST(op_pipe, | )

__dyn_OP_MUTATING(op_plus_equals, +=)
__dyn_OP_MUTATING(op_minus_equals, -=)
__dyn_OP_MUTATING(op_star_equals, *=)
__dyn_OP_MUTATING(op_slash_equals, /=)
__dyn_OP_MUTATING(op_percent_equals, %=)
__dyn_OP_MUTATING(op_caret_equals, ^=)
__dyn_OP_MUTATING(op_ampersand_equals, &=)
__dyn_OP_MUTATING(op_pipe_equals, |=)
__dyn_OP_MUTATING(op_less_less, << )
__dyn_OP_MUTATING(op_greater_greater, >> )
__dyn_OP_MUTATING(op_less_less_equals, <<=)
__dyn_OP_MUTATING(op_greater_greater_equals, >>=)

#undef __dyn_OP_CONST
#undef __dyn_OP_MUTATING
#undef __dyn_OP0

/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, template <typename, typename, typename...> typename Trait, typename... Args>
inline auto unerase_cast(dyn<Trait, Args...> const& o) {
    return unerase_cast_if<U>(o.proxy_, o.v_table_);
}
/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, template <typename, typename, typename...> typename Trait, typename... Args>
inline auto unerase_cast_if(dyn<Trait, Args...> const& o) {
    return unerase_cast_if<U>(o.proxy_, o.v_table_);
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
inline auto get_v_table(dyn<Trait, Args...> const& any) {
    return v_table_cast<typename dyn<Trait, Args...>::v_table_t>(any.v_table_);
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
auto release_v_table(dyn<Trait, Args...>& any) { return std::exchange(any.v_table_, nullptr); }


struct type_info_{
    using type = std::type_info const*;
    template<typename Concrete>
    static std::type_info const* init(auto){ return &typeid(Concrete); }
};
struct is_derived_from_{
    using type = anyxx::is_derived_from_t;
    template<typename Concrete, typename VTable>
    static anyxx::is_derived_from_t init(VTable* ){
        return +[]([[maybe_unused]] const std::type_info& from) {
            return false;
        //return VTable::static_is_derived_from(
        //    from);  // not yet implemented! must build a list of base classes in
        //            // the vtable and check if from is in that list
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

struct dispatch_table {
    using type = anyxx::dispatch_table_t*;
    template<typename Concrete, typename VTable>
    static auto init(VTable*){
        return anyxx::dispatch_table_instance<VTable, Concrete>();
    }
};

// clang-format off
template <is_trait Trait = declaration, typename Self = declaration>
struct moveable {
    using model_size [[= v_table_data]] = anyxx26::model_size;
    using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
    using destructor [[= v_table_data]] = anyxx26::destructor;
    using default_proxy_t = anyxx::val<>;
    template <is_trait ApplyTrait, typename ApplSelf>
    using self_apply = moveable<ApplyTrait, ApplSelf>;
};
    template <is_trait Trait = declaration, typename Self = declaration>
    struct copyable : moveable<Trait, Self> {
        using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = copyable<ApplyTrait, ApplSelf>;
    };
template <is_trait Trait = declaration, typename Self = declaration>
struct save_observable {
    using type_info_ [[= v_table_data]] = anyxx26::type_info_;
    template <is_trait ApplyTrait, typename ApplSelf>
    using self_apply = save_observable<ApplyTrait, ApplSelf>;
};
    template <is_trait Trait = declaration, typename Self = declaration>
    struct save_moveable : save_observable<Trait, Self> {
        using model_size [[= v_table_data]] = anyxx26::model_size;
        using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
        using destructor [[= v_table_data]] = anyxx26::destructor;
        using default_proxy_t = anyxx::val<>;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = save_moveable<ApplyTrait, ApplSelf>;
    };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct save_copyable : save_moveable<Trait, Self> {
            using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = save_copyable<ApplyTrait, ApplSelf>;
        };
    template <is_trait Trait = declaration, typename Self = declaration >
    struct dynamic_castable : save_observable<Trait, Self> {
        using is_derived_from_ [[= v_table_data]] = anyxx26::is_derived_from_;
        using meta_data_ [[= v_table_data]] = anyxx26::meta_data_;
        template <is_trait ApplyTrait, typename ApplSelf>
        using self_apply = dynamic_castable<ApplyTrait, ApplSelf>;
    };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct dynamic_deletable : dynamic_castable<Trait, Self> {
            using delete_ [[= v_table_data]] = anyxx26::delete_;
            using default_proxy_t = anyxx::shared;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = dynamic_deletable<ApplyTrait, ApplSelf>;
        };
            template <is_trait Trait = declaration, typename Self = declaration>
            struct dynamic_smart_ptr : dynamic_deletable<Trait, Self> {
                using model_size [[= v_table_data]] = anyxx26::model_size;
                using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
                template <is_trait ApplyTrait, typename ApplSelf>
                using self_apply = dynamic_smart_ptr<ApplyTrait, ApplSelf>;
            };
        template <is_trait Trait = declaration, typename Self = declaration>
        struct dynamic_moveable : dynamic_castable<Trait, Self> {
            using model_size [[= v_table_data]] = anyxx26::model_size;
            using move_constructor [[= v_table_data]] = anyxx26::move_constructor;
            using destructor [[= v_table_data]] = anyxx26::destructor;
            using default_proxy_t = anyxx::val<>;
            template <is_trait ApplyTrait, typename ApplSelf>
            using self_apply = dynamic_moveable<ApplyTrait, ApplSelf>;
        };
            template <is_trait Trait = declaration, typename Self = declaration>
            struct dynamic_copyable : dynamic_moveable<Trait, Self> {
                using copy_constructor [[= v_table_data]] = anyxx26::copy_constructor;
                template <is_trait ApplyTrait, typename ApplSelf>
                using self_apply = dynamic_copyable<ApplyTrait, ApplSelf>;
            };
// clang-format on

}  // namespace anyxx26

/// \def ANY26_REGISTER_MODEL
/// \brief Register a model class for a specific trait. Must
/// be in global namespace.
/// \param class_ The model class with fully qualified name. Must be
/// parenthesized
/// \param trait_ Name of the trait.
/// \param ... Optional template parameters for the trait.
///
/// See also \ref casts.
#define ANY26_REGISTER_MODEL(class_, interface_, ...)                         \
  namespace {                                                                 \
  static auto __ = anyxx::bind_v_table_to_meta_data                           \
        <anyxx26::v_table<interface_ __VA_OPT__(, __VA_ARGS__)>, ANYXX_UNPAREN(class_)>(); \
  }
