#pragma once

#include <anypp/trait.hpp>

namespace anypp {

template <voidness VOIDNESS>
using observer = VOIDNESS;
using const_observer = observer<const_void>;
using mutable_observer = observer<mutable_void>;

template <typename ERASED_DATA, voidness VOIDNESS>
struct observer_trait {
  using void_t = VOIDNESS;
  static constexpr bool is_const = is_const_void<void_t>;
  static constexpr bool is_constructibile_from_const = is_const;
  static constexpr bool is_owner = false;
  static constexpr bool is_weak = false;

  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }
  static VOIDNESS value(const auto& ptr) { return ptr; }

  template <typename CONSTRUCTED_WITH>
  using unerased = CONSTRUCTED_WITH;

  template <typename V>
  static auto erase(V& v) {
    static_assert(!std::is_const_v<std::remove_reference_t<V>>);
    return ERASED_DATA(static_cast<VOIDNESS>(&v));
  }
  template <typename V>
  static auto erase(const V& v)
    requires(is_const)
  {
    return ERASED_DATA(static_cast<VOIDNESS>(&v));
  }
};

template <>
struct trait<const_observer>
    : observer_trait<const_observer, const_observer> {};
template <>
struct trait<mutable_observer>
    : observer_trait<mutable_observer, mutable_observer> {};

static_assert(trait<const_observer>::is_const);
static_assert(!trait<mutable_observer>::is_const);
static_assert(is_erased_data<const_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<mutable_observer>);
static_assert(is_erased_data<const_observer>);

}  // namespace anypp