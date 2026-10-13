#pragma once

#include <bit_factory/anyxx/any_concept.hpp>
#include <bit_factory/anyxx/proxies/proxy_concept.hpp>
#include <bit_factory/anyxx/voidness.hpp>
#include <bit_factory/v23/v_table_holder/v_table_holder_forward.hpp>

namespace anyxx {

template <typename TraitDerived, is_proxy ProxyDerived, typename TraitBase,
          is_proxy ProxyBase>
constexpr bool is_any_derived_from_v<any<TraitDerived, ProxyDerived>,
                                     any<TraitBase, ProxyBase>> =
    std::derived_from<typename any<TraitDerived, ProxyDerived>::v_table_t,
                      typename any<TraitBase, ProxyBase>::v_table_t>;

/// \brief The core class template to control dispatch for external
/// polymorphism
///
/// To control the behavior, `any` provides two template parameters: \ref
/// Proxy and \ref Trait. Imagine this as a combination of a `std::any` and
/// several `std::function`s.
///
/// With the Proxy template parameter, you control whether this `any` behaves
/// like a copying function, a move-only function, a reference function, or if
/// the target object is captured concretely inside of `any`.
///
/// With the Trait template parameter, you specify the member functions of a
/// captured object which can be invoked on this `any`.
///
/// \tparam Proxy Specifies the lifetime of the captured object. Any++
/// provides
/// \ref using_, \ref cref, \ref mutref, \ref shared, \ref weak, \ref unique,
/// and
/// \ref value. All Proxy classes must conform to the \ref is_proxy concept.
/// \tparam Trait Specifies the functionality of this any. A class of this
/// type is normally provided via a \ref TRAIT or \ref ANY macro. See there
/// for examples. If the proxy is dynamic (i.e., type erased), the Trait must
/// conform to the \ref has_v_table concept (that means: must provide a
/// v-Table).
template <typename Trait, is_proxy Proxy>
class ANYXX_USE_EBO any : public v_table_holder<Proxy, Trait>, public Trait {
 public:
  using proxy_t = Proxy;
  using proxy_trait_t = proxy_trait<proxy_t>;
  using void_t = typename proxy_trait_t::void_t;
  using v_table_holder_t = v_table_holder<Proxy, Trait>;
  using trait_t = Trait;
  using v_table_t = typename v_table_holder_t::v_table_t;
  using T = proxy_trait_t::static_dispatch_t;
  using model_map_t = typename trait_t::template static_dispatch_map_t<T>;
  using rep_type = typename model_map_t::rep_type;
  using proxy_impl_t = typename proxy_trait_t::template proxy_impl<rep_type>;
  static constexpr bool is_dyn = anyxx::is_dyn<Proxy>;

 protected:
  proxy_impl_t proxy_{};

 public:
  any()
    requires proxy_trait_t::allow_any_default_constructibile
  {}

  // cppcheck-suppress-begin noExplicitConstructor
  /// Type-erasing constructor for lifetime owning proxies. The concrete
  /// behavior is controlled by the proxy via its corresponding \ref
  /// proxy_trait. See \ref using_, \ref shared, \ref weak, \ref unique, and
  /// \ref value.
  template <typename ConstructedWith>
  explicit(false) any(ConstructedWith&& constructed_with)  // NOLINT
    requires constructibile_for<ConstructedWith, proxy_impl_t, trait_t> &&
             (!std::same_as<any, std::decay_t<ConstructedWith>>) &&
             (!is_lifetime_bound<proxy_impl_t>)
      : proxy_(
            erased<proxy_t>(std::forward<ConstructedWith>(constructed_with))) {
    v_table_holder_t::template init_v_table<Proxy, ConstructedWith>();
  }
  /// Type-erasing constructor for borrowing proxies. The concrete behavior is
  /// controlled by the proxy via its corresponding \ref proxy_trait. See \ref
  /// cref, \ref mutref
  template <typename ConstructedWith>
  explicit(false)
      any(ConstructedWith&& constructed_with LIFETIMEBOUND)  // NOLINT
    requires constructibile_for<ConstructedWith, proxy_impl_t, trait_t> &&
             (!std::same_as<any, std::decay_t<ConstructedWith>>) &&
             (is_lifetime_bound<proxy_impl_t>)
      : proxy_(
            erased<proxy_t>(std::forward<ConstructedWith>(constructed_with))) {
    v_table_holder_t::template init_v_table<proxy_impl_t, ConstructedWith>();
  }
  // cppcheck-suppress-end noExplicitConstructor
  /// Type-erasing constructor. The concrete behavior is controlled by the
  /// proxy. The value `v` will be forwarded to the managed storage.
  /// \tparam V The type of the object to be forwarded into the managed
  /// storage. Usually deduced.
  /// \param std::in_place_t Tag to select in-place construction. (On call
  /// site, use `std::in_place`.)
  /// \param v The object to be forwarded into the managed storage.
  template <typename V>
    requires(!is_lifetime_bound<Proxy>)
  any(std::in_place_t, V&& v)
      : proxy_(
            proxy_trait<proxy_impl_t>::construct_in_place(std::forward<V>(v))) {
    v_table_holder_t::template init_v_table<proxy_impl_t, V>();
  }
  /// Type erasing constructor, the concrete behavior is controled by the
  /// proxy
  /// \param std::in_place_type_t<T> Tag to select in-place construction of
  /// `T`
  /// \param args The arguments to construct the object of type T with, will
  /// be forwarded.
  template <typename T, typename... Args>
    requires(!is_lifetime_bound<Proxy>)
  any(std::in_place_type_t<T>, Args&&... args)
      : proxy_(proxy_trait_t::template construct_type_in_place<T>(
            std::forward<Args>(args)...)) {
    v_table_holder_t::template init_v_table<proxy_impl_t, T>();
  }

  ~any() {
    proxy_trait_t::destroy(proxy_, v_table_holder_t::get_v_table_ptr());
  }

  any(const any& other)
    requires(is_dyn && can_copy_construct_from<proxy_trait_t, v_table_t>)
      : v_table_holder_t(other.get_v_table_ptr()) {
    proxy_trait_t::copy_construct_from(proxy_, nullptr, other.proxy_,
                                       other.get_v_table_ptr());
  }
  any(const any& other)
    requires(!is_dyn && can_copy_construct_from<proxy_trait_t, v_table_t>)
      : v_table_holder_t(other.get_v_table_ptr()), proxy_(other.proxy_) {}
  any& operator=(any const& other)
    requires(can_copy_construct_from<proxy_trait_t, v_table_t>)
  {
    if (this == &other) return *this;
    auto const v_table_ptr = v_table_holder_t::get_v_table_ptr();
    proxy_trait_t::copy_construct_from(proxy_, v_table_ptr, other.proxy_,
                                       other.get_v_table_ptr());
    v_table_holder_t::set_v_table_ptr(other.get_v_table_ptr());
    return *this;
  }

  template <is_any Other>
  explicit(false) any(const Other& other)  // NOLINT(noExplicitConstructor)
    requires(borrowable_from<proxy_t, typename Other::proxy_t,
                                   typename Other::v_table_t> &&
             (!anyxx::is_dyn<Proxy> || is_any_derived_from<Other, any>))
      : v_table_holder_t(other.get_v_table_ptr()),
        proxy_(borrow_proxy_as<Proxy>(other.proxy_, other.get_v_table_ptr())) {}
  template <is_any Other>
  any& operator=(Other const& other)
    requires(borrowable_from<proxy_t, typename Other::proxy_t,
                                   typename Other::v_table_t> &&
             (!anyxx::is_dyn<Proxy> || is_any_derived_from<Other, any>))
  {
    v_table_holder_t::set_v_table_ptr(other.get_v_table_ptr());
    proxy_ = borrow_proxy_as<Proxy>(other.proxy_, other.get_v_table_ptr());
    return *this;
  }

  template <is_proxy OtherErasedData>
    requires(moveable_from<proxy_t, OtherErasedData>)
  explicit any(OtherErasedData&& proxy, v_table_t* v_table) noexcept
      : v_table_holder_t(v_table) {
    proxy_trait_t::move_to(proxy_, nullptr, std::move(proxy), v_table);
  }
  template <is_proxy OtherErasedData>
    requires(moveable_from<proxy_t, OtherErasedData> && !is_dyn)
  explicit any(OtherErasedData&& proxy, v_table_t* v_table) noexcept
      : v_table_holder_t(v_table), proxy_(std::move(proxy)) {}
  template <is_any Other>
  explicit(false) any(Other&& other) noexcept  // NOLINT(noExplicitConstructor)
    requires(moveable_from<proxy_t, typename Other::proxy_t> &&
             (!anyxx::is_dyn<Proxy> || is_any_derived_from<Other, any>))
      : any(std::move(other.proxy_), other.release_v_table()) {}
  template <is_any Other>
  any& operator=(Other&& other) noexcept
    requires(moveable_from<proxy_t, typename Other::proxy_t> &&
             (!anyxx::is_dyn<Proxy> || is_any_derived_from<Other, any>))
  {
    proxy_trait_t::move_to(proxy_, v_table_holder_t::get_v_table_ptr(),
                           std::move(other.proxy_), other.get_v_table_ptr());
    v_table_holder_t::set_v_table_ptr(other.release_v_table());
    return *this;
  }

  template <is_any Friend>
  friend inline auto& get_proxy(Friend const& any);
  template <is_any Friend>
  friend inline auto& get_proxy(Friend& any);
  template <is_any Friend>
  friend inline decltype(auto) move_proxy(Friend&& any);
  template <is_any Friend>
  friend inline auto get_proxy_ptr_const(Friend const& any);
  template <is_any Friend>
  friend inline auto get_proxy_ptr(Friend&& any);

  template <typename OtherTrait, is_proxy Other>
  friend class any;

  template <typename Friend>
    requires is_any<Friend> && Friend::is_dyn
  friend inline auto get_v_table(Friend const& any);

  template <is_any To, is_any From>
  friend inline To unchecked_downcast_to(From from)
    requires(is_any_derived_from<To, From>);

  explicit operator bool() const
    requires proxy_trait_t::allow_any_default_constructibile
  {
    if constexpr (!voidness<typename proxy_trait_t::static_dispatch_t>) {
      if constexpr (is_type_class<proxy_t>) {
        return true;
      } else {
        return proxy_.value_;
      }
    } else {
      auto p = get_proxy_ptr(*this);
      return p != nullptr;
    }
  }

  template <typename Self>
  Self operator++(this Self& self, int)
    requires(is_dyn && is_op_pre_increment_v_table<v_table_t>) ||
            (!is_dyn && is_op_pre_increment_model_map<model_map_t, T>)

  {
    auto r = self;
    ++(self);
    return r;
  }

  template <typename Self>
  Self& operator++(this Self&& self)
    requires is_dyn && is_op_pre_increment_v_table<v_table_t>
  {
    get_v_table(self)->op_pre_increment(
        get_proxy_ptr(std::forward<Self>(self)));
    return std::forward<Self>(self);
  }
  template <typename Self>
  Self& operator++(this Self&& self)
    requires(!is_dyn && is_op_pre_increment_model_map<model_map_t, T>)
  {
    model_map_t{}.op_pre_increment(get_proxy_value(std::forward<Self>(self)));
    return std::forward<Self>(self);
  }

  template <typename Self>
  Self operator--(this Self& self, int)
    requires(is_dyn && is_op_pre_decrement_v_table<v_table_t>) ||
            (!is_dyn && is_op_pre_decrement_model_map<model_map_t, T>)

  {
    auto r = self;
    --(self);
    return r;
  }
  template <typename Self>
  Self& operator--(this Self&& self)
    requires is_dyn && is_op_pre_decrement_v_table<v_table_t>
  {
    get_v_table(self)->op_pre_decrement(
        get_proxy_ptr(std::forward<Self>(self)));
    return std::forward<Self>(self);
  }
  template <typename Self>
  Self& operator--(this Self&& self)
    requires(!is_dyn && is_op_pre_decrement_model_map<model_map_t, T>)
  {
    model_map_t{}.op_pre_decrement(get_proxy_value(std::forward<Self>(self)));
    return std::forward<Self>(self);
  }
};

/// A template to get an \ref any trait for a type
/// See also \ref using_::as.
template <typename Type, typename Trait>
using use_as = any<Trait, using_<Type>>;

/// A template to get an \ref any trait for a type
/// See also \ref using_cref::as.
template <typename Type, typename Trait>
using use_as_cref = any<Trait, using_cref<Type>>;

/// A template to get an templatetd \ref any trait for a type
/// See also \ref using_::as.
template <typename Type, template <typename...> typename Trait, typename... Ts>
using use_as_ = any<Trait<Ts...>, using_<Type>>;

/// A template to get an templatetd \ref any trait for a type
/// See also \ref using_cref::as.
template <typename Type, template <typename...> typename Trait, typename... Ts>
using use_as_cref_ = any<Trait<Ts...>, using_cref<Type>>;

/// A factory function to bind an object as a model to an \ref any with a \ref
/// trait.
/// See also \ref using_::as.
template <typename Trait, typename T>
auto trait_as(T&& v) {
    return any<Trait, using_<std::decay_t<T>>>{std::forward<T>(v)};
}

/// A factory function to bind a const& as a model to an \ref any with a \ref
/// trait.
/// See also \ref using_cref::as.
template <typename Trait, typename T>
auto trait_as_cref(T const& v) {
    return any<Trait, using_cref<std::decay_t<T>>>{v};
}

/// Helper type alias template to get a \ref trait_class for a type
/// dispatch. Has no data member, so it doesn't capture any value and has no
/// size
///
/// \tparam Type The captured type
template <typename Type, typename Trait>
using any_trait_class = any<Trait, trait_class<Type>>;

/// A object template to get a \ref trait_class object for a type as a \ref
/// trait.
/// See also \ref using_::as.
template <typename Type, typename Trait>
static inline any_trait_class<Type, Trait> trait_class_;

}  // namespace anyxx