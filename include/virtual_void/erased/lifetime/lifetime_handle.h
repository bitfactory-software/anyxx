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
// template <typename T>
// static DATA_PTR construct_from(T&&);

template <typename DATA_PTR>
struct data_trait_base {
  template <typename... ARGS>
  auto operator()(ARGS&&... args) {
    return lifetime_handle<DATA_PTR>(std::forward<ARGS>(args)...);
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
  template <typename T>
  lifetime_handle(T&& v)
    requires(!std::derived_from<std::decay_t<T>, lifetime_handle> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void>)
      : DATA_PTR(std::in_place_type<T>), data_(&v) {}
  lifetime_handle(DATA_PTR data) : data_(std::move(data)) {}

  void const* data() const
    requires is_const
  {
    return trait_t::data(data_);
  }
  void* data() const
    requires !is_const
  {
    return trait_t::data(data_);
  }
  auto meta() const { return trait_t::meta(data_); }
  explicit operator bool() const { trait_t::has_value(data_); }
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

template <typename T, typename DATA_PTR>
struct typed_lifetime_handle : public lifetime_handle<DATA_PTR> {
  using conrete_t = std::remove_reference_t<T>;
  using lifetime_handle_t = lifetime_handle<DATA_PTR>;
  using lifetime_handle_t::lifetime_handle_t;
  typed_lifetime_handle(const typed_lifetime_handle&) = default;
  typed_lifetime_handle(typed_lifetime_handle&) = default;
  typed_lifetime_handle(typed_lifetime_handle&&) = default;
  typed_lifetime_handle& operator=(const typed_lifetime_handle&) = default;
  typed_lifetime_handle& operator=(typed_lifetime_handle&) = default;
  typed_lifetime_handle& operator=(typed_lifetime_handle&&) = default;
  typed_lifetime_handle(T&& v)
      : lifetime_handle<DATA_PTR>(std::forward<T>(v)) {}
  conrete_t const& operator*() const {
    return *static_cast<conrete_t*>(this->data());
  }
  conrete_t const* operator->() const {
    return static_cast<conrete_t*>(this->data());
  }
  conrete_t& operator*() const
    requires !lifetime_handle_t::is_const
  {
    return *static_cast<conrete_t*>(this->data());
  }
  conrete_t* operator->() const
    requires !lifetime_handle_t::is_const
  {
    return static_cast<conrete_t*>(this->data());
  }

 private:
  typed_lifetime_handle(const lifetime_handle_t& o)
      : lifetime_handle<DATA_PTR>(o) {}
  template <typename T, typename DATA_PTR>
  friend auto as(lifetime_handle<DATA_PTR>&& source);
};

template <typename T, typename DATA_PTR>
auto as(lifetime_handle<DATA_PTR>&& source) {
  return typed_lifetime_handle<T, DATA_PTR>{std::move(source)};
}
// template <typename T, typename DATA_PTR>
// auto as(const lifetime_handle<DATA_PTR>& source) {
//   return typed_lifetime_handle<T, DATA_PTR>{source};
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