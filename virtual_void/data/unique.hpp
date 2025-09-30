#pragma once

#include <memory>
#include <virtual_void/data/trait.hpp>

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

template <>
struct trait<data::unique> {
  using void_t = void*;
  template <typename V>
  using typed_t = std::decay_t<V>;
  static constexpr bool is_constructibile_from_const = true;

  static void* value(const auto& ptr) { return ptr.get(); }
  static bool has_value(const auto& ptr) { return static_cast<bool>(ptr); }

  template <typename CONSTRUCTED_WITH>
  using unerased = typename CONSTRUCTED_WITH::element_type;

  template <typename V>
  static auto construct_from(std::unique_ptr<V>&& v) {
    return data::move_to_unique(std::move(v));
  }
};

static_assert(is_erased_data<unique>);
}  // namespace virtual_void::data
