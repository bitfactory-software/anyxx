#pragma once

#include <memory>
#include <virtual_void/data/trait.hpp>

namespace virtual_void::data {
using shared_const = std::shared_ptr<void const>;

template <>
struct erased_data_trait<shared_const> {
  using void_t = void const*;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;

  static void const* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename CONSTRUCTED_WITH>
  using unerased_type = std::decay_t<typename CONSTRUCTED_WITH::element_type>;

  template <typename V>
  static auto construct_from(std::shared_ptr<V> const& v) {
    return static_pointer_cast<void const>(v);
  }
};

static_assert(is_erased_data<shared_const>);
}  // namespace virtual_void::data