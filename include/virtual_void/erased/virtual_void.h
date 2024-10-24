#pragma once

#include <stdexcept>
#include <type_traits>

#include "concept.h"
#include "data/has_no_meta/has_no_meta.h"
#include "data/has_type_info/has_type_info.h"

namespace virtual_void::erased {

template <typename VOID>
bool is_const_void;
template <>
constexpr bool is_const_void<void*> = false;
template <>
constexpr bool is_const_void<void const*> = true;

template <typename DATA_PTR>
struct virtual_void;

template <typename DATA_PTR>
struct data_trait_base {
  template <typename FROM>
  auto operator()(FROM&& from) {
    return virtual_void<DATA_PTR>(std::forward<FROM>(from));
  }  // for migration to lifteime_handle! delete after migration!
};

template <typename DATA_PTR>
struct virtual_void {
  DATA_PTR ptr_ = nullptr;

  using data_t = DATA_PTR;
  using trait_t = data_trait<DATA_PTR>;
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
      : ptr_(trait_t::construct_from(std::forward<V>(v))) {}
  template <typename V, typename... ARGS>
  virtual_void(std::in_place_type_t<V>, ARGS&&... args)
      : ptr_(trait_t::construct_in_place(std::in_place_type<V>,
                                         std::forward<ARGS>(args)...)) {}
  virtual_void(DATA_PTR data) : ptr_(std::move(data)) {}

  // only for migration to lifetime handle, remove and replace use to "value()"!
  void const* data() const
    requires is_const
  {
    return value();
  }
  void* data() const
    requires !is_const
  {
    return value();
  }

  void const* value() const
    requires is_const
  {
    return trait_t::value(ptr_);
  }
  void* value() const
    requires !is_const
  {
    return trait_t::value(ptr_);
  }
  auto meta() const { return trait_t::meta(ptr_); }
  explicit operator bool() const { return trait_t::has_value(ptr_); }
};

template <typename U, typename DATA_PTR>
auto reconcrete_cast(virtual_void<DATA_PTR> const& o) {
  return static_cast<U const*>(o.data());
}
template <typename U, typename DATA_PTR>
auto reconcrete_cast(virtual_void<DATA_PTR> const& o)
  requires !virtual_void<DATA_PTR>::is_const
{
  return static_cast<U*>(o.data());
}

template <typename V, typename DATA_PTR>
struct virtual_typed : public virtual_void<DATA_PTR> {
  using value_t = V;
  using lifetime_handle_t = virtual_void<DATA_PTR>;
  using lifetime_handle_t::lifetime_handle_t;

  virtual_typed(const virtual_typed&) = default;
  virtual_typed(virtual_typed&) = default;
  virtual_typed(virtual_typed&&) = default;
  virtual_typed& operator=(const virtual_typed&) = default;
  virtual_typed& operator=(virtual_typed&) = default;
  virtual_typed& operator=(virtual_typed&&) = default;
  template <typename FROM>
  explicit virtual_typed(FROM&& from)
    requires(!std::derived_from<std::decay_t<FROM>, lifetime_handle_t> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<V>>, void>)
      : lifetime_handle_t(std::in_place_type<V>, std::forward<FROM>(from)) {}
  template <typename... ARGS>
  virtual_typed(std::in_place_t, ARGS&&... args)
      : lifetime_handle_t(std::in_place_type<V>, std::forward<ARGS>(args)...) {}
  explicit virtual_typed(DATA_PTR data) : virtual_void(std::move(data)) {}

  value_t const& operator*() const {
    return *static_cast<value_t*>(this->value());
  }
  value_t const* operator->() const {
    return static_cast<value_t*>(this->value());
  }
  value_t& operator*() const
    requires !lifetime_handle_t::is_const
  {
    return *static_cast<value_t*>(this->value());
  }
  value_t* operator->() const
    requires !lifetime_handle_t::is_const
  {
    return static_cast<value_t*>(this->value());
  }

 private:
  explicit virtual_typed(lifetime_handle_t&& virtual_void)
      : lifetime_handle_t(std::move(virtual_void)) {}
  template <typename V, typename DATA_PTR>
  friend auto as(virtual_void<DATA_PTR> source);
};

template <typename V, typename DATA_PTR>
auto as(virtual_void<DATA_PTR> source) {
  // if constexpr (virtual_void<DATA_PTR>::is_const) {
  //   static_assert(std::is_const_v<V>);
  // }
  return virtual_typed<V, DATA_PTR>{std::move(source)};
}

}  // namespace virtual_void::erased