#pragma once

#include <memory>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {
using shared_const = std::shared_ptr<void const>;
}  // namespace virtual_void::data

namespace virtual_void {
template <>
struct erased_data_trait<data::shared_const> {
  using void_t = void const*;
  template <typename V>
  using typed_t = const std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;

  static void const* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename CONSTRUCTED_WITH>
  struct unerased_type_impl {
    using type = CONSTRUCTED_WITH;
  };
  template <typename CONSTRUCTED_WITH>
  struct unerased_type_impl<std::shared_ptr<CONSTRUCTED_WITH>> {
    using type = CONSTRUCTED_WITH;
  };
  template <typename CONSTRUCTED_WITH>
  using unerased_type = unerased_type_impl<CONSTRUCTED_WITH>::type;

  template <typename V>
  struct construct_from_impl {
    template <typename X>
    auto operator()(X&& v) {
      return construct<X>(std::forward<X>(v));
    }
  };
  template <typename V>
  struct construct_from_impl<std::shared_ptr<V>> {
    template <typename X>
    auto operator()(X&& v) {
      return static_pointer_cast<void const>(std::forward<X>(v));
    }
  };
  template <typename V>
  struct construct_from_impl<std::shared_ptr<V const>> {
    template <typename X>
    auto operator()(X&& v) {
      return static_pointer_cast<void const>(std::forward<X>(v));
    }
  };
  template <typename V>
  static auto construct_from(V&& v) {
    return construct_from_impl<std::decay_t<V>>{}(std::forward<V>(v));
  }
  template <typename V, typename... ARGS>
  static auto construct(ARGS&&... args) {
    return static_pointer_cast<void const>(
        std::make_shared<typed_t<V>>(std::forward<ARGS>(args)...));
  }
};
}  // namespace virtual_void

namespace virtual_void::data {
static_assert(is_erased_data<shared_const>);
}