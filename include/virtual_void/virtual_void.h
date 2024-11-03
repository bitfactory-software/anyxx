#pragma once

#include <concepts>
#include <type_traits>

#include "forward.h"

namespace virtual_void {

template <class BASE, class DERIVED>
concept base_of = std::derived_from<DERIVED, BASE>;

template <typename DATA>
struct virtual_void_trait;

template <class META>
concept is_meta = requires(META meta) {
  { meta.get_meta() } -> std::convertible_to<META const*>;
  { meta.type_info() } -> std::convertible_to<type_info_ptr>;
};

template <class PTR>
concept has_virtual_void_trait =
    requires(PTR ptr) { typename virtual_void_trait<PTR>::void_t; };

template <class E>
concept is_virtual_void = has_virtual_void_trait<E>;

template <class E>
concept is_virtual_typed = requires(E e) {
  typename E::void_t;
  typename E::data_t;
  typename E::trait_t;
  //  typename E::make_erased;
  // typename E::trait_t;
  { E::is_const } -> std::convertible_to<bool>;
  //  { E::make_erased()(i) } -> base_of<E>;
  //	{ get_data(e.data_) } -> std::convertible_to<typename E::void_t>;
  //  { e.meta()->type_info() } -> std::convertible_to<std::type_info const*>;
};

template <typename VOID>
struct is_const_void_;
template <>
struct is_const_void_<void*> : std::false_type {};
template <>
struct is_const_void_<void const*> : std::true_type {};

template <typename VOID>
concept is_const_void = is_const_void_<VOID>::value;

template <typename DATA>
concept is_const_data =
    is_const_void<typename virtual_void_trait<DATA>::void_t>;

template <typename VOID, typename DATA>
concept const_correct_for_virtual_void_data =
    ((is_const_void<VOID> == is_const_data<DATA>) || (!is_const_data<DATA>));

template <typename VOID, typename VIRTUAL_VOID>
concept const_correct_for_virtual_void =
    (const_correct_for_virtual_void_data<VOID, VIRTUAL_VOID>);

template <typename V, typename DATA>
struct virtual_typed;

template <is_virtual_void VIRTUAL_VOID, typename FROM>
VIRTUAL_VOID erased(FROM&& from) {
  return virtual_void_trait<VIRTUAL_VOID>::construct_from(
      std::forward<FROM>(from));
}
template <is_virtual_void VIRTUAL_VOID, typename FROM>
VIRTUAL_VOID erase_to(FROM&& from) {
  if constexpr (is_virtual_void<std::remove_reference_t<FROM>>) {
    return from;
  } else {
    return erased<VIRTUAL_VOID>(from);
  }
}
template <is_virtual_void VIRTUAL_VOID, typename V, typename... ARGS>
VIRTUAL_VOID erased_in_place(ARGS&&... args) {
  return virtual_void_trait<VIRTUAL_VOID>::construct_in_place(
      std::in_place_type<V>, std::forward<ARGS>(args)...);
}

struct virtual_void_default_unerase {
  template <typename CONSTRUCTED_WITH>
  using unerased_type = CONSTRUCTED_WITH;
};

template <typename T>
struct static_cast_uneraser {
  using type = T;
  auto operator()(auto from) { return static_cast<T*>(from); }
};
template <is_virtual_void VIRTUAL_VOID, typename CONSTRUCTED_WITH>
auto unerase() {
  using constructed_with_t = std::remove_cvref_t<CONSTRUCTED_WITH>;
  using trait_t = virtual_void_trait<VIRTUAL_VOID>;
  if constexpr (is_virtual_void<constructed_with_t>) {
    using value_t = typename constructed_with_t::value_t;
    return static_cast_uneraser<value_t>();
  } else {
    using value_t = trait_t::template unerased_type<constructed_with_t>;
    if constexpr (is_const_data<VIRTUAL_VOID>) {
      return static_cast_uneraser<value_t const>();
    } else {
      return static_cast_uneraser<value_t>();
    }
  }
}

template <is_virtual_void VIRTUAL_VOID>
bool has_data(VIRTUAL_VOID const& vv) {
  return virtual_void_trait<VIRTUAL_VOID>::has_value(vv);
}
template <is_virtual_void VIRTUAL_VOID>
void const* get_data(VIRTUAL_VOID const& vv)
  requires std::same_as<void const*,
                        typename virtual_void_trait<VIRTUAL_VOID>::void_t>
{
  return virtual_void_trait<VIRTUAL_VOID>::value(vv);
}
template <is_virtual_void VIRTUAL_VOID>
void* get_data(VIRTUAL_VOID const& vv)
  requires std::same_as<void*,
                        typename virtual_void_trait<VIRTUAL_VOID>::void_t>
{
  return virtual_void_trait<VIRTUAL_VOID>::value(vv);
}
template <is_virtual_void VIRTUAL_VOID>
auto get_meta(VIRTUAL_VOID const& vv) {
  return virtual_void_trait<VIRTUAL_VOID>::meta(vv);
}

template <typename U, is_virtual_void VIRTUAL_VOID>
auto unerase_cast(VIRTUAL_VOID const& o) {
  return static_cast<U const*>(get_data(o));
}
template <typename U, is_virtual_void VIRTUAL_VOID>
auto unerase_cast(VIRTUAL_VOID const& o)
  requires(!is_const_data<VIRTUAL_VOID>)
{
  return static_cast<U*>(get_data(o));
}

template <typename V, typename DATA>
struct virtual_typed {
  DATA data_;

  using data_t = DATA;
  using trait_t = virtual_void_trait<DATA>;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using value_t = V;

  virtual_typed(const virtual_typed&) = default;
  virtual_typed(virtual_typed&) = default;
  virtual_typed(virtual_typed&&) = default;
  virtual_typed& operator=(const virtual_typed&) = default;
  virtual_typed& operator=(virtual_typed&) = default;
  virtual_typed& operator=(virtual_typed&&) = default;
  template <typename FROM>
  explicit virtual_typed(FROM&& from)
    requires(!std::same_as<std::decay_t<FROM>, virtual_typed> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<V>>, void>)
      : data_(trait_t::construct_in_place(std::in_place_type<V>,
                                          std::forward<FROM>(from))) {}
  template <typename... ARGS>
  virtual_typed(std::in_place_t, ARGS&&... args)
      : data_(trait_t::construct_in_place(std::in_place_type<V>,
                                          std::forward<ARGS>(args)...)) {}

  explicit virtual_typed(DATA data) : data_(std::move(data)) {}

  value_t const& operator*() const {
    return *static_cast<value_t*>(get_data(data_));
  }
  value_t const* operator->() const {
    return static_cast<value_t*>(get_data(data_));
  }
  value_t& operator*() const
    requires !is_const
  {
    return *static_cast<value_t*>(get_data(data_));
  }
  value_t* operator->() const
    requires !is_const
  {
    return static_cast<value_t*>(get_data(data_));
  }
};

template <typename V, typename VIRTUAL_VOID>
bool has_data(virtual_typed<V, VIRTUAL_VOID> const& vv) {
  return has_data(vv.data_);
}
template <typename V, typename VIRTUAL_VOID>
void const* get_data(virtual_typed<V, VIRTUAL_VOID> const& vv)
  requires std::same_as<void const*,
                        typename virtual_void_trait<VIRTUAL_VOID>::void_t>
{
  return get_data(vv.data_);
}
template <typename V, typename VIRTUAL_VOID>
void* get_data(virtual_typed<V, VIRTUAL_VOID> const& vv)
  requires std::same_as<void*,
                        typename virtual_void_trait<VIRTUAL_VOID>::void_t>
{
  return get_data(vv.data_);
}
template <typename V, typename VIRTUAL_VOID>
auto get_meta(virtual_typed<V, VIRTUAL_VOID> const& vv) {
  return virtual_void_trait<VIRTUAL_VOID>::meta(vv.data_);
}

template <typename V, is_virtual_void DATA>
auto as(DATA source) {
  return virtual_typed<V, DATA>{std::move(source)};
}

template <typename TO, typename FROM, typename DATA>
auto as(virtual_typed<FROM, DATA> source)
  requires std::convertible_to<FROM*, TO*>
{
  if constexpr (virtual_typed<FROM, DATA>::is_const) {
    return virtual_typed<TO const, DATA>{std::move(source.data_)};
  } else {
    return virtual_typed<TO, DATA>{std::move(source.data_)};
  }
}

}  // namespace virtual_void