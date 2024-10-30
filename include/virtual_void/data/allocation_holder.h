#pragma once

#include "../forward.h"

namespace virtual_void::erased::data {

template <typename META_DATA>
struct allocation_base : META_DATA {
  template <typename V>
  allocation_base(std::in_place_type_t<V>) : META_DATA(std::in_place_type<V>) {}
  void* value();
  void const* value() const;
};

template <typename V, typename META_DATA>
struct allocation_holder : allocation_base<META_DATA> {
  using meta_data_t = META_DATA;
  using base_t = allocation_base<META_DATA>;
  using value_t = V;
  value_t value_;
  template <typename... ARGS>
  allocation_holder(std::in_place_t in_place, ARGS&&... args)
      : base_t(std::in_place_type<value_t>),
        value_(std::forward<ARGS>(args)...) {}
};

template <typename META_DATA>
void* allocation_base<META_DATA>::value() {
  return &static_cast<allocation_holder<int, META_DATA>*>(this)->value_;
};
template <typename META_DATA>
void const* allocation_base<META_DATA>::value() const {
  return &static_cast<allocation_holder<int, META_DATA> const*>(this)->value_;
};

template <typename TO, typename META_DATA>
TO const* unerase_cast(allocation_base<META_DATA> const& holded) {
  return static_cast<TO const*>(holded.value());
}
template <typename TO, typename META_DATA>
TO* unerase_cast(allocation_base<META_DATA>& holded) {
  return static_cast<TO*>(holded.value());
}

}  // namespace virtual_void::erased::data