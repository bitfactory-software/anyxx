#pragma once

#include <memory>
#include <virtual_void/trait.hpp>

namespace virtual_void {
using shared_const = std::shared_ptr<void const>;
using weak = std::weak_ptr<void const>;

template <>
struct trait<shared_const> {
  using void_t = void const*;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  static constexpr bool is_owner = true;
  static constexpr bool is_weak = false;

  static void const* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename CONSTRUCTED_WITH>
  using unerased = std::decay_t<typename CONSTRUCTED_WITH::element_type>;

  template <typename V>
  static auto erase(std::shared_ptr<V> const& v) {
    return static_pointer_cast<void const>(v);
  }
};

template <>
struct trait<weak> {
  using void_t = void const*;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;
  static constexpr bool is_owner = false;
  static constexpr bool is_weak = true;

  static void const* value(const auto& ptr) { return nullptr; }
  static bool has_value(const auto& ptr) { return !ptr.expired(); }

  template <typename CONSTRUCTED_WITH>
  using unerased = std::decay_t<typename CONSTRUCTED_WITH::element_type>;

  template <typename V>
  static auto erase(std::weak_ptr<V> const& v) {
    return static_pointer_cast<void const>(v);
  }
};

static_assert(is_erased_data<shared_const>);
}  // namespace virtual_void