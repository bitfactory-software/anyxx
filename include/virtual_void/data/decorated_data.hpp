#pragma once

#include "../virtual_void.hpp"

namespace virtual_void::data {

template <typename META_DATA>
struct decoration_base : META_DATA {
  template <typename V>
  decoration_base(std::in_place_type_t<V>) : META_DATA(std::in_place_type<V>) {}
  void* value();
  void const* value() const;
};

template <typename V, typename META_DATA>
struct decorated_data : decoration_base<META_DATA> {
  using meta_data_t = META_DATA;
  using base_t = decoration_base<META_DATA>;
  using value_t = V;
  value_t value_;
  template <typename... ARGS>
  decorated_data(std::in_place_t in_place, ARGS&&... args)
      : base_t(std::in_place_type<value_t>),
        value_(std::forward<ARGS>(args)...) {}
};

template <typename META_DATA>
void* decoration_base<META_DATA>::value() {
  return &static_cast<decorated_data<int, META_DATA>*>(this)->value_;
};
template <typename META_DATA>
void const* decoration_base<META_DATA>::value() const {
  return &static_cast<decorated_data<int, META_DATA> const*>(this)->value_;
};

template <typename TO, typename META_DATA>
TO const* unchecked_unerase_cast(decoration_base<META_DATA> const& holded) {
  return static_cast<TO const*>(holded.value());
}
template <typename TO, typename META_DATA>
TO* unchecked_unerase_cast(decoration_base<META_DATA>& holded) {
  return static_cast<TO*>(holded.value());
}
template <typename TO, typename META_DATA>
TO const* unerase_cast(decoration_base<META_DATA> const& holded) {
  check_type_match<TO>(holded.get_meta());
  return unchecked_unerase_cast<TO>(holded);
}
template <typename TO, typename META_DATA>
TO* unerase_cast(decoration_base<META_DATA>& holded) {
  check_type_match<TO>(holded.get_meta());
  return unchecked_unerase_cast<TO>(holded);
}

}  // namespace virtual_void::data