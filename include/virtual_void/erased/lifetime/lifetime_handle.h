#pragma once

#include <stdexcept>
#include <type_traits>

#include "../concept.h"
#include "../data/has_no_meta.h"
#include "../data/has_type_info.h"

namespace virtual_void::erased {

template <typename VOID>
bool is_const_void;
template <>
constexpr bool is_const_void<void*> = false;
template <>
constexpr bool is_const_void<void const*> = true;

template <typename DATA_PTR>
struct lifetime_handle;

template <typename DATA_PTR>
struct data_trait;
// using void_t = const void*;
// static auto data(const auto& ptr);
// static auto data(auto& ptr);
// static auto meta(const auto& ptr);
// static bool has_value(const auto& ptr);
// template <typename V>
// static DATA_PTR construct_from(V&&);

template <typename DATA_PTR>
struct data_trait_base {
  template <typename FROM>
  auto operator()(FROM&& from) {
    return lifetime_handle<DATA_PTR>(std::forward<FROM>(from));
  }  // for migration to lifteime_handle! delete after migration!
};

template <typename DATA_PTR>
struct lifetime_handle {
  DATA_PTR data_ = nullptr;

  using data_t = DATA_PTR;
  using trait_t = data_trait<DATA_PTR>;
  using void_t = trait_t::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using make_erased = trait_t;

  lifetime_handle(const lifetime_handle&) = default;
  lifetime_handle(lifetime_handle&) = default;
  lifetime_handle(lifetime_handle&&) = default;
  lifetime_handle& operator=(const lifetime_handle&) = default;
  lifetime_handle& operator=(lifetime_handle&) = default;
  lifetime_handle& operator=(lifetime_handle&&) = default;
  template <typename V>
  explicit lifetime_handle(V&& v)
    requires(!std::derived_from<std::decay_t<V>, lifetime_handle> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<V>>, void>)
      : data_(trait_t::construct_from(std::forward<V>(v))) {}
  template <typename V, typename... ARGS>
  lifetime_handle(std::in_place_type_t<V>, ARGS&&... args)
      : data_(trait_t::construct_in_place(std::in_place_type<V>,
                                          std::forward<ARGS>(args)...)) {}
  explicit lifetime_handle(DATA_PTR data) : data_(std::move(data)) {}

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
    return trait_t::value(data_);
  }
  void* value() const
    requires !is_const
  {
    return trait_t::value(data_);
  }
  auto meta() const { return trait_t::meta(data_); }
  explicit operator bool() const { return trait_t::has_value(data_); }
};

template <typename U, typename DATA_PTR>
auto reconcrete_cast(lifetime_handle<DATA_PTR> const& o) {
  return static_cast<U const*>(o.data());
}
template <typename U, typename DATA_PTR>
auto reconcrete_cast(lifetime_handle<DATA_PTR> const& o)
  requires !lifetime_handle<DATA_PTR>::is_const
{
  return static_cast<U*>(o.data());
}

template <typename V, typename DATA_PTR>
struct typed_lifetime_handle : public lifetime_handle<DATA_PTR> {
  using value_t = V;
  using lifetime_handle_t = lifetime_handle<DATA_PTR>;
  using lifetime_handle_t::lifetime_handle_t;

  typed_lifetime_handle(const typed_lifetime_handle&) = default;
  typed_lifetime_handle(typed_lifetime_handle&) = default;
  typed_lifetime_handle(typed_lifetime_handle&&) = default;
  typed_lifetime_handle& operator=(const typed_lifetime_handle&) = default;
  typed_lifetime_handle& operator=(typed_lifetime_handle&) = default;
  typed_lifetime_handle& operator=(typed_lifetime_handle&&) = default;
  template <typename FROM>
  explicit typed_lifetime_handle(FROM&& from)
    requires(!std::derived_from<std::decay_t<V>, lifetime_handle_t> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<V>>, void>)
      : lifetime_handle_t(std::in_place_type<V>, std::forward<FROM>(from)) {}
  explicit typed_lifetime_handle(DATA_PTR data) : lifetime_handle(std::move(data)) {}

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
  explicit typed_lifetime_handle(lifetime_handle_t&& lifetime_handle)
      : lifetime_handle_t(std::move(lifetime_handle)) {}
  template <typename V, typename DATA_PTR>
  friend auto as(lifetime_handle<DATA_PTR>&& source);
};

template <typename V, typename DATA_PTR>
auto as(lifetime_handle<DATA_PTR>&& source) {
  return typed_lifetime_handle<V, DATA_PTR>{std::move(source)};
}
// template <typename V, typename DATA_PTR>
// auto as(const lifetime_handle<DATA_PTR>& source) {
//   return typed_lifetime_handle<V, DATA_PTR>{source};
// }

// static_assert(is_erased_lifetime_holder<mutable_lifetime_handle<>>);
// static_assert(is_erased_lifetime_holder<const_lifetime_handle<>>);
// static_assert(
//     is_erased_lifetime_holder<mutable_lifetime_handle<data::has_type_info>>);
// static_assert(
//     is_erased_lifetime_holder<const_lifetime_handle<data::has_type_info>>);
// static_assert(is_erased_lifetime_holder<
//               typed_lifetime_handle<int const, data::has_type_info>>);
// static_assert(
//     is_erased_lifetime_holder<typed_lifetime_handle<int,
//     data::has_type_info>>);

}  // namespace virtual_void::erased