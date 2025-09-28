#pragma once

#include <memory>
#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

template <typename T>
auto erased_delete() {
  return +[](void* data) { delete static_cast<T*>(data); };
}

using void_deleter_t = void (*)(void*);
using unique = std::unique_ptr<void, void_deleter_t>;

template <typename T>
auto move_to_unique(std::unique_ptr<T> p) {
  return unique(p.release(), erased_delete<T>());
}

template <typename T, typename... ARGS>
auto make_unique(ARGS&&... args) {
  return move_to_unique(std::make_unique<T>(std::forward<ARGS>(args)...));
}

inline unique unique_nullptr() {
  return {nullptr, [](auto) {}};
}

};  // namespace virtual_void::data

namespace virtual_void {
template <>
struct erased_data_trait<data::unique> {
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
      return data::move_to_unique(std::move(v));
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

namespace virtual_void::data {
static_assert(is_erased_data<unique>);
}  // namespace virtual_void::data