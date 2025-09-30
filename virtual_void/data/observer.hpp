#pragma once

#include <virtual_void/data/trait.hpp>

namespace virtual_void::data {

template <voidness VOIDNESS>
using observer = VOIDNESS;
using const_observer = observer<const_void>;
using mutable_observer = observer<mutable_void>;

template <typename ERASED_DATA, typename VV_VOID>
struct observer_trait : virtual_void_default_unerase {
  using void_t = VV_VOID;
  static constexpr bool is_const = is_const_void<void_t>;
  static constexpr bool is_constructibile_from_const = is_const;

  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }
  static VV_VOID value(const auto& ptr) { return ptr; }

  template <typename V>
  static auto construct_from(V& v) {
    static_assert(!std::is_const_v<std::remove_reference_t<V>>);
    return ERASED_DATA(static_cast<VV_VOID>(&v));
  }
  template <typename V>
  static auto construct_from(const V& v)
    requires(is_const)
  {
    return ERASED_DATA(static_cast<VV_VOID>(&v));
  }
};

template <>
struct erased_data_trait<data::const_observer>
    : observer_trait<data::const_observer, data::const_observer> {};
template <>
struct erased_data_trait<data::mutable_observer>
    : observer_trait<data::mutable_observer, data::mutable_observer> {};

static_assert(erased_data_trait<const_observer>::is_const);
static_assert(!erased_data_trait<mutable_observer>::is_const);
static_assert(is_erased_data<const_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<const_observer>);

}  // namespace virtual_void::data