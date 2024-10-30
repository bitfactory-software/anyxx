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
  //  typename E::make_erased;
  // typename E::trait_t;
  { E::is_const } -> std::convertible_to<bool>;
  //  { E::make_erased()(i) } -> base_of<E>;
  { e.data() } -> std::convertible_to<typename E::void_t>;
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

template <typename VOID, typename DATA>
concept const_correct_for_virtual_void_data =
    ((std::is_const_v<VOID> ==
      std::is_const_v<typename virtual_void_trait<DATA>::void_t>) ||
     (!std::is_const_v<typename virtual_void_trait<DATA>::void_t>));

template <typename VOID, typename VIRTUAL_VOID>
concept const_correct_for_virtual_void =
    (const_correct_for_virtual_void_data<VOID, typename VIRTUAL_VOID::data_t>);

template <typename DATA>
class virtual_void;
template <typename V, typename DATA>
class virtual_typed;

template <typename DATA>
class virtual_void {
  DATA data_ = nullptr;

 public:
  using data_t = DATA;
  using trait_t = virtual_void_trait<DATA>;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using make_erased = trait_t;

  virtual_void(const virtual_void&) = default;
  virtual_void(virtual_void&) = default;
  virtual_void(virtual_void&&) noexcept = default;
  virtual_void& operator=(const virtual_void&) = default;
  virtual_void& operator=(virtual_void&) = default;
  virtual_void& operator=(virtual_void&&) noexcept = default;
  template <typename V>
  virtual_void(V&& v)
    requires(!std::derived_from<std::decay_t<V>, virtual_void> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<V>>, void>)
      : data_(trait_t::construct_from(std::forward<V>(v))) {}
  template <typename V, typename... ARGS>
  virtual_void(std::in_place_type_t<V>, ARGS&&... args)
      : data_(trait_t::construct_in_place(std::in_place_type<V>,
                                          std::forward<ARGS>(args)...)) {}
  virtual_void(DATA data) : data_(std::move(data)) {}

  void const* data() const
    requires is_const
  {
    return trait_t::value(data_);
  }
  void* data() const
    requires !is_const
  {
    return trait_t::value(data_);
  }

  auto meta() const { return trait_t::meta(data_); }
  explicit operator bool() const { return trait_t::has_value(data_); }

  template <typename TO, typename FROM, typename DATA>
  friend auto as(virtual_typed<FROM, DATA> source)
    requires std::convertible_to<FROM*, TO*>;
};

template <typename U, typename DATA>
auto reconcrete_cast(virtual_void<DATA> const& o) {
  return static_cast<U const*>(o.data());
}
template <typename U, typename DATA>
auto reconcrete_cast(virtual_void<DATA> const& o)
  requires !virtual_void<DATA>::is_const
{
  return static_cast<U*>(o.data());
}

template <typename V, typename DATA>
class virtual_typed : public virtual_void<DATA> {
 public:
  using value_t = V;
  using virtual_void_t = virtual_void<DATA>;
  using virtual_void_t::virtual_void_t;

  virtual_typed(const virtual_typed&) = default;
  virtual_typed(virtual_typed&) = default;
  virtual_typed(virtual_typed&&) = default;
  virtual_typed& operator=(const virtual_typed&) = default;
  virtual_typed& operator=(virtual_typed&) = default;
  virtual_typed& operator=(virtual_typed&&) = default;
  template <typename FROM>
  explicit virtual_typed(FROM&& from)
    requires(!std::derived_from<std::decay_t<FROM>, virtual_void_t> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<V>>, void>)
      : virtual_void_t(std::in_place_type<V>, std::forward<FROM>(from)) {}
  template <typename... ARGS>
  virtual_typed(std::in_place_t, ARGS&&... args)
      : virtual_void_t(std::in_place_type<V>, std::forward<ARGS>(args)...) {}
  explicit virtual_typed(DATA data) : virtual_void_t(std::move(data)) {}

  value_t const& operator*() const {
    return *static_cast<value_t*>(this->data());
  }
  value_t const* operator->() const {
    return static_cast<value_t*>(this->data());
  }
  value_t& operator*() const
    requires !virtual_void_t::is_const
  {
    return *static_cast<value_t*>(this->data());
  }
  value_t* operator->() const
    requires !virtual_void_t::is_const
  {
    return static_cast<value_t*>(this->data());
  }

 private:
  explicit virtual_typed(virtual_void_t&& virtual_void)
      : virtual_void_t(std::move(virtual_void)) {}
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