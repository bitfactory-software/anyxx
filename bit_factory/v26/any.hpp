#pragma once

#include <array>
#include <bit_factory/anyxx.hpp>
#include <bit_factory/v26/any/bases.hpp>
#include <bit_factory/v26/any/facade.hpp>
#include <bit_factory/v26/any/keywords.hpp>
#include <bit_factory/v26/any/v_table.hpp>
#include <bit_factory/v26/trait_translation/deduced_typenames.hpp>
#include <bit_factory/v26/trait_translation/trait_facade_decorator.hpp>
#include <meta>
#include <utility>
#include <vector>

namespace anyxx26 {

template <typename Any>
concept is_any = anyxx::is_any<Any> && 
    requires { typename Any::trait_declaration_t; };

template <template <typename, typename, typename...> typename Trait, typename... Args>
constexpr decltype(auto) preprocess_constructed_with(auto&& constructed_with) {
    using ConstructedWith = decltype(constructed_with);
    using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
    if constexpr (requires (ConstructedWith constructed_with){ { trait_declaration_t::preprocess_constructed_with(constructed_with) }; }) {
        return trait_declaration_t::preprocess_constructed_with(std::forward<ConstructedWith>(constructed_with));
    } else {
        return constructed_with;
    }
}

template <typename ConstructedWith, template <typename, typename, typename...> typename Trait, typename... Args>
using preprocess_constructed_with_t =
    std::decay_t<decltype(preprocess_constructed_with<Trait, Args...>(std::declval<ConstructedWith>()))>;

template <template <typename, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct any_base : deduced_typenames<Trait, Args...> {
  using trait_declaration_t = anyxx26::trait_declaration_t<Trait, Args...>;
  using dyn_self_t = any<Trait, Args...>;
  using dyn_self_cref_t = any<Trait, anyxx::cref, Args...>;
  using dyn_self_mutref_t = any<Trait, anyxx::mutref, Args...>;
  using proxy_t = Proxy;
  using proxy_trait_t = anyxx::proxy_trait<proxy_t>;
  using void_t = typename proxy_trait_t::void_t;
  using v_table_t = v_table<Trait, Args...>;
  inline static constexpr bool is_dyn = true; 

  v_table_t* v_table_;
  proxy_t proxy_{};

  constexpr any_base()
    requires proxy_trait_t::allow_any_default_constructibile
  {}

  template <typename ConstructedWith>
  explicit(false) constexpr any_base(ConstructedWith&& constructed_with)  // NOLINT
    requires anyxx::constructibile_for<ConstructedWith, proxy_t,
                                       any_base<Trait, proxy_t, Args...>>
      : v_table_(v_table_instance<Trait, std::decay_t<preprocess_constructed_with_t<ConstructedWith, Trait, Args...>>, Args...>()),
        proxy_(anyxx::erased<proxy_t>(
            preprocess_constructed_with<Trait, Args...>(std::forward<ConstructedWith>(constructed_with)))) {}

  template <typename V>
    requires(!anyxx::is_lifetime_bound<proxy_t>)
  constexpr any_base(std::in_place_t, V&& v)
      : v_table_(v_table_instance<Trait, V, Args...>()),
        proxy_(proxy_trait_t::construct_in_place(std::forward<V>(v))) {}

  template <typename T, typename... ConstructWithArgs>
    requires(!anyxx::is_lifetime_bound<proxy_t>)
  constexpr any_base(std::in_place_type_t<T>, ConstructWithArgs&&... args)
      : v_table_(v_table_instance<Trait, T, Args...>()),
        proxy_(proxy_trait_t::template construct_type_in_place<T>(
            std::forward<ConstructWithArgs>(args)...)) {}

  constexpr ~any_base() { proxy_trait_t::destroy(proxy_, v_table_); }

  constexpr any_base(const any_base& other)
    requires(anyxx::can_copy_construct_from<proxy_trait_t, v_table_t>)
      : v_table_(other.v_table_) {
    proxy_trait_t::copy_construct_from(proxy_, nullptr, other.proxy_,
                                       other.v_table_);
  }
  constexpr any_base& operator=(any_base const& other)
    requires(anyxx::can_copy_construct_from<proxy_trait_t, v_table_t>)
  {
    if (this == &other) return *this;
    auto v_table_ptr = v_table_;
    proxy_trait_t::copy_construct_from(proxy_, v_table_ptr, other.proxy_,
                                       other.v_table_);
    return *this;
  }
  constexpr any_base(any_base&& other) noexcept  // NOLINT(noExplicitConstructor)
    requires(anyxx::moveable_from<proxy_t, proxy_t>)
      : any_base(std::move(other.proxy_), release_v_table(other)) {}
  constexpr any_base& operator=(any_base&& other) noexcept
    requires(anyxx::moveable_from<proxy_t, proxy_t>)
  {
    proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_), other.v_table_);
    v_table_ = release_v_table(other);
    return *this;
  }

  template <anyxx26::is_any Other>
  explicit(false) constexpr any_base(const Other& other)  // NOLINT(noExplicitConstructor)
      requires(anyxx::proxy_borrowable_from<proxy_t, typename Other::proxy_t, typename Other::v_table_t> &&
        anyxx::is_any_derived_from_v<Other, any_base>)
      : v_table_(v_table_cast<v_table_t>(other.v_table_)),
      proxy_(anyxx::borrow_proxy_as<proxy_t>(other.proxy_, other.v_table_)) {
  }
  template <anyxx26::is_any Other>
  constexpr any_base& operator=(Other const& other)
      requires(anyxx::proxy_borrowable_from<proxy_t, typename Other::proxy_t, typename Other::v_table_t> &&
        anyxx::is_any_derived_from_v<Other, any_base>)
  {
      v_table_ = v_table_cast<v_table_t>(other.v_table_);
      proxy_ = anyxx::borrow_proxy_as<proxy_t>(other.proxy_, other.v_table_);
      return *this;
  }

  template <anyxx::is_proxy OtherErasedData>
      requires(anyxx::moveable_from<proxy_t, OtherErasedData>)
  explicit constexpr any_base(OtherErasedData&& proxy, v_table_t* v_table) noexcept
      : v_table_(v_table) {
      proxy_trait_t::move_to(proxy_, nullptr, std::move(proxy), v_table);
  }
  template <anyxx26::is_any Other>
  explicit(false) constexpr any_base(Other&& other) noexcept  // NOLINT(noExplicitConstructor)
      requires(anyxx::moveable_from<proxy_t, typename Other::proxy_t> &&
        anyxx::is_any_derived_from_v<Other, any_base>)
      : any_base(std::move(other.proxy_), v_table_cast<v_table_t>(release_v_table(other))) {
  }
  template <anyxx26::is_any Other>
  constexpr any_base& operator=(Other&& other) noexcept
      requires(anyxx::moveable_from<proxy_t, typename Other::proxy_t> &&
        anyxx::is_any_derived_from_v<Other, any_base>)
  {
      proxy_trait_t::move_to(proxy_, v_table_, std::move(other.proxy_), other.v_table_);
      v_table_ = v_table_cast<v_table_t>(release_v_table(other));
      return *this;
  }

  friend constexpr auto release_v_table(any_base& self) { return std::exchange(self.v_table_, nullptr); }
};

template <template <is_trait, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct any_with_facade : any_base<Trait, Proxy, Args...>, [:make_dyn_facade<any_base<Trait, Proxy, Args...>>() :] {
    using any_base<Trait, Proxy, Args...>::any_base;
};
template <template <is_trait, typename, typename...> typename Trait, anyxx::is_proxy Proxy, typename... Args>
struct any<Trait, Proxy, Args...> : any_with_facade<Trait, Proxy, Args...>, trait_facade_decorator_t<any<Trait, Proxy, Args...>, Trait, Args...> {
    using any_with_facade<Trait, Proxy, Args...>::any_with_facade;
};
template <template <is_trait, typename, typename...> typename Trait>
struct any<Trait> : any<Trait, default_proxy_t<Trait>> {
    using any<Trait, default_proxy_t<Trait>>::any;
};
template <template <is_trait, typename, typename...> typename Trait, typename Arg0, typename... Args>
struct any<Trait, Arg0, Args...> : any<Trait, default_proxy_t<Trait, Arg0, Args...>, Arg0, Args...> {
    using any<Trait, default_proxy_t<Trait, Arg0, Args...>, Arg0, Args...>::any;
};

#define __dyn_OP_CONST(function, op) \
template <template <typename, typename, typename...> typename Trait, typename Other, typename... Args> \
    requires (requires(any<Trait, Args...> const& lhs, Other const& rhs){ {lhs.function(rhs)}; }) \
constexpr decltype(auto) operator op (any<Trait, Args...> const& lhs, Other const& rhs) { \
    return lhs.function(rhs); \
}
#define __dyn_OP_MUTATING(function, op) \
template <template <typename, typename, typename...> typename Trait, typename Other, typename... Args> \
    requires (requires(any<Trait, Args...>& lhs, Other const& rhs){ {lhs.function(rhs)}; }) \
constexpr decltype(auto) operator op (any<Trait, Args...>& lhs, Other const& rhs) { \
    return lhs.function(rhs); \
}
#define __dyn_OP0(function, op) \
template <template <typename, typename, typename...> typename Trait, typename... Args> \
    requires (requires(any<Trait, Args...> const& lhs){ {lhs.function()}; }) \
constexpr decltype(auto) operator op (any<Trait, Args...> const& lhs) { \
    return lhs.function(); \
}
#define __dyn_OP0_MUTATING(function, op) \
template <template <typename, typename, typename...> typename Trait, typename... Args> \
    requires (requires(any<Trait, Args...>& lhs){ {lhs.function()}; }) \
constexpr decltype(auto) operator op (any<Trait, Args...>& lhs) { \
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
#undef __dyn_OP0_MUTATING


/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, template <typename, typename, typename...> typename Trait, typename... Args>
inline constexpr auto unerase_cast(any<Trait, Args...> const& o) {
    return anyxx::unerase_cast_if<U>(o.proxy_, o.v_table_);
}
/// \brief Safe downcast to an unerased type using runtime information from
/// the v-Tables.
/// \ingroup casts
template <typename U, template <typename, typename, typename...> typename Trait, typename... Args>
inline constexpr auto unerase_cast_if(any<Trait, Args...> const& o) {
    return anyxx::unerase_cast_if<U>(o.proxy_, o.v_table_);
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
inline constexpr auto get_v_table(any<Trait, Args...> const& any) {
    return v_table_cast<typename any<Trait, Args...>::v_table_t>(any.v_table_);
}

template <template <typename, typename, typename...> typename Trait, typename... Args>
inline constexpr auto release_v_table(any<Trait, Args...>& any) { return std::exchange(any.v_table_, nullptr); }

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
