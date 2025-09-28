#pragma once

#include <virtual_void/data/erased_unique_ptr.hpp>
#include <virtual_void/data/make_unique.hpp>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void {

struct unique_trait {
  using void_t = void*;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;

  static void* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename CONSTRUCTED_WITH>
  struct unerased_type_impl {
    using type = CONSTRUCTED_WITH;
  };
  template <typename CONSTRUCTED_WITH>
  struct unerased_type_impl<std::unique_ptr<CONSTRUCTED_WITH>> {
    using type = CONSTRUCTED_WITH;
  };
  template <typename CONSTRUCTED_WITH>
  using unerased_type = unerased_type_impl<CONSTRUCTED_WITH>::type;

  template <typename V>
  struct construct_from_impl {
    template <typename X>
    auto operator()(X&& v) {
      return data::make_unique<typed_t<V>>(std::forward<X>(v));
    }
  };
  template <typename V>
  struct construct_from_impl<std::unique_ptr<V>> {
    template <typename X>
    auto operator()(X&& v) {
      return data::move_to_unique_void(std::move(v));
    }
  };

  template <typename V>
  static auto construct_from(V&& v) {
    return construct_from_impl<typed_t<V>>{}(std::forward<V>(v));
  }
  template <typename V, typename... ARGS>
  static auto construct_in_place(std::in_place_type_t<V>, ARGS&&... args) {
    return data::make_unique<typed_t<V>>(std::forward<ARGS>(args)...);
  }
};

}  // namespace virtual_void