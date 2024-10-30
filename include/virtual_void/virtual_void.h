#pragma once

#include <concepts>
#include <type_traits>

#include "data/has_no_meta/meta.h"
#include "data/has_type_info/meta.h"
#include "forward.h"

namespace virtual_void {

template <class BASE, class DERIVED>
concept base_of = std::derived_from<DERIVED, BASE>;

template <class E>
concept is_virtual_void = requires(E e, int i) {
  typename E::void_t;
  typename E::data_t;
  typename E::trait_t;
  //  typename E::make_erased;
  // typename E::trait_t;
  { E::is_const } -> std::convertible_to<bool>;
  //  { E::make_erased()(i) } -> base_of<E>;
  { get_data(e) } -> std::convertible_to<typename E::void_t>;
  //  { e.meta()->type_info() } -> std::convertible_to<std::type_info const*>;
};

template <typename DATA>
struct virtual_void_trait;

template <class PTR>
concept has_virtual_void_trait =
    requires(PTR ptr) { typename virtual_void_trait<PTR>::void_t; };

template <is_virtual_void VIRTUAL_VOID, typename FROM>
VIRTUAL_VOID erase_to(FROM&& from) {
  if constexpr (is_virtual_void<std::remove_reference_t<FROM>>) {
    return from;
  } else {
    return VIRTUAL_VOID::make_erased()(std::forward<FROM>(from));
  }
}

template <typename T>
struct static_cast_uneraser {
  using type = T;
  auto operator()(auto from) { return static_cast<T*>(from); }
};

template <is_virtual_void VIRTUAL_VOID, typename CONSTRUCTED_WITH>
auto unerase() {
  using constructed_with_t = std::remove_cvref_t<CONSTRUCTED_WITH>;
  if constexpr (is_virtual_void<constructed_with_t>) {
    using trait_t = typename VIRTUAL_VOID::trait_t;
    using value_t = typename constructed_with_t::value_t;
    return static_cast_uneraser<value_t>();
  } else {
    if constexpr (VIRTUAL_VOID::is_const) {
      return static_cast_uneraser<constructed_with_t const>();
    } else {
      return static_cast_uneraser<constructed_with_t>();
    }
  }
}

template <typename DATA>
concept is_const_data =
    is_const_void<typename virtual_void_trait<DATA>::void_t>::value;

template <typename VOID, typename DATA>
concept const_correct_for_virtual_void_data =
    ((is_const_void<VOID>::value == is_const_data<DATA>) ||
     (!is_const_data<DATA>));

template <typename VOID, typename VIRTUAL_VOID>
concept const_correct_for_virtual_void =
    (const_correct_for_virtual_void_data<VOID, typename VIRTUAL_VOID::data_t>);

template <typename V, typename DATA>
class virtual_typed;

template <typename DATA>
using virtual_void = DATA;
// public:
//  DATA data_ = nullptr;
//
// public:
//  using data_t = DATA;
//  using trait_t = virtual_void_trait<DATA>;
//  using void_t = trait_t::void_t;
//  static constexpr bool is_const = is_const_void<void_t>::value;
//  using make_erased = trait_t;
//
//  virtual_void(const virtual_void&) = default;
//  virtual_void(virtual_void&) = default;
//  virtual_void(virtual_void&&) noexcept = default;
//  virtual_void& operator=(const virtual_void&) = default;
//  virtual_void& operator=(virtual_void&) = default;
//  virtual_void& operator=(virtual_void&&) noexcept = default;
//  template <typename V>
//  virtual_void(V&& v)
//    requires(!std::derived_from<std::decay_t<V>, virtual_void> &&
//             !std::same_as<std::decay_t<std::remove_pointer_t<V>>, void>)
//      : data_(trait_t::construct_from(std::forward<V>(v))) {}
//  template <typename V, typename... ARGS>
//  virtual_void(std::in_place_type_t<V>, ARGS&&... args)
//      : data_(trait_t::construct_in_place(std::in_place_type<V>,
//                                          std::forward<ARGS>(args)...)) {}
//  virtual_void(DATA data) : data_(std::move(data)) {}
//};

template <typename VIRTUAL_VOID>
bool has_data(VIRTUAL_VOID const& vv) {
  return VIRTUAL_VOID::trait_t::has_value(vv.data_);
}
template <typename VIRTUAL_VOID>
void const* get_data(VIRTUAL_VOID const& vv)
  requires std::same_as<void const*, typename virtual_void_trait<
                                         typename VIRTUAL_VOID::data_t>::void_t>
{
  return VIRTUAL_VOID::trait_t::value(vv.data_);
}
template <typename VIRTUAL_VOID>
void* get_data(VIRTUAL_VOID const& vv)
  requires std::same_as<
      void*, typename virtual_void_trait<typename VIRTUAL_VOID::data_t>::void_t>
{
  return VIRTUAL_VOID::trait_t::value(vv.data_);
}
template <typename VIRTUAL_VOID>
auto get_meta(VIRTUAL_VOID const& vv) {
  return virtual_void_trait<typename VIRTUAL_VOID::data_t>::meta(vv.data_);
}

template <typename U, typename DATA>
auto reconcrete_cast(virtual_void<DATA> const& o) {
  return static_cast<U const*>(get_data(o));
}
template <typename U, typename DATA>
auto reconcrete_cast(virtual_void<DATA> const& o)
  requires !virtual_void<DATA>::is_const
{
  return static_cast<U*>(get_data(o));
}

template <typename V, typename DATA>
class virtual_typed {
 private:
  DATA data_;

 public:
  using data_t = DATA;
  using trait_t = virtual_void_trait<DATA>;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>::value;
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
      : data_(trait_t::construct_in_place(std::in_place_type<FROM>,
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

 private:
  template <typename V, typename DATA>
  friend auto as(virtual_void<DATA> source);
};

template <typename V, typename DATA>
auto as(virtual_void<DATA> source) {
  // if constexpr (virtual_void<DATA>::is_const) {
  //   static_assert(std::is_const_v<V>);
  // }
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