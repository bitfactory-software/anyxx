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
bool is_const_void<void*> = false;
template <>
bool is_const_void<void const*> = true;

template <typename DATA_PTR>
struct lifetime_handle;

template <typename DATA_PTR>
struct data_trait {
  using void_t = const void*;
  static auto data(const auto& ptr);
  static auto data(auto& ptr);
  static auto meta(const auto& ptr);
  template <typename T>
  static DATA_PTR construct_from(T&&);
  template <typename ARG>
  auto operator()(ARG&& arg) {
    return lifetime_handle<DATA_PTR>(std::forward<ARG>(arg));
  }  // for migration to lifteime_handle! delete after migration!
};

template <typename DATA_PTR>
struct lifetime_handle {
  DATA_PTR data_ = nullptr;

  using void_t = data_trait<DATA_PTR>::void_t;
  static constexpr bool is_const = is_const_void<void_t>;
  using make_erased = data_trait<DATA_PTR>;

  auto data() const { return data_trait::data(data_); }
  auto data()
    requires !is_const
  {
    return data_trait::data(data_);
  }
  auto meta() const { return data_trait::meta(data_); }

  lifetime_handle(const lifetime_handle&) = default;
  lifetime_handle(lifetime_handle&) = default;
  lifetime_handle(lifetime_handle&&) = default;
  template <typename T>
  lifetime_handle(T&& v)
    requires(!std::derived_from<std::decay_t<T>, lifetime_handle> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void>)
      : DATA_PTR(std::in_place_type<T>), data_(&v) {}
  lifetime_handle(DATA_PTR data) : data_(std::move(data)) {}
};

template <typename U, typename DATA_PTR>
auto reconcrete_cast(lifetime_handle<DATA_PTR> const& o) {
  return static_cast<U const*>(o.data());
}
template <typename U, typename DATA_PTR>
auto reconcrete_cast(lifetime_handle<DATA_PTR> const& o)
  requires !lifetime_handle<DATA_PTR>::is_const
{
  return static_cast<U const*>(o.data());
}

template <typename T, typename DATA_PTR>
struct typed_lifetime_handle : public lifetime_handle<DATA_PTR> {
  using conrete_t = std::remove_reference_t<T>;
  using lifetime_handle_t = lifetime_handle<DATA_PTR>;
  typed_lifetime_handle(const typed_lifetime_handle&) = default;
  typed_lifetime_handle(typed_lifetime_handle&) = default;
  typed_lifetime_handle(typed_lifetime_handle&&) = default;
  typed_lifetime_handle(const lifetime_handle_t& o)
      : lifetime_handle<DATA_PTR>(o) {}
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
};

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