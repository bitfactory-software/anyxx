#pragma once

#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

struct decoration_base {
  template <typename V>
  decoration_base(std::in_place_type_t<V>) {}
  void* value();
  void const* value() const;
};

template <typename V>
struct decorated_data : decoration_base {
  using base_t = decoration_base;
  using value_t = V;
  value_t value_;
  template <typename... ARGS>
  decorated_data(std::in_place_t in_place, ARGS&&... args)
      : base_t(std::in_place_type<value_t>),
        value_(std::forward<ARGS>(args)...) {}
};

inline void* decoration_base::value() {
  return &static_cast<decorated_data<int>*>(this)->value_;
};
inline void const* decoration_base::value() const {
  return &static_cast<decorated_data<int> const*>(this)->value_;
};

template <typename TO>
TO const* unchecked_unerase_cast(decoration_base const& holded) {
  return static_cast<TO const*>(holded.value());
}
template <typename TO>
TO* unchecked_unerase_cast(decoration_base& holded) {
  return static_cast<TO*>(holded.value());
}

}  // namespace virtual_void::data