#pragma once

#include "../../forward.h"

namespace virtual_void::erased::data {

#ifdef _DEBUG
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL virtual
#else
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL
#endif  // DEBUG

template <typename META_DATA>
struct base : META_DATA {
  template <typename V>
  base(std::in_place_type_t<V>) : META_DATA(std::in_place_type<V>) {}
  void* value();
  void const* value() const;
};

template <typename V, typename META_DATA>
struct holder : base<META_DATA> {
  using meta_data_t = META_DATA;
  using base_t = base<META_DATA>;
  using value_t = V;
  value_t value_;
  template <typename... ARGS>
  holder(std::in_place_t in_place, ARGS&&... args)
      : base_t(std::in_place_type<value_t>),
        value_(std::forward<ARGS>(args)...) {}
};

template <typename META_DATA>
void* base<META_DATA>::value() {
  return &static_cast<holder<int, META_DATA>*>(this)->value_;
};
template <typename META_DATA>
void const* base<META_DATA>::value() const {
  return &static_cast<holder<int, META_DATA> const*>(this)->value_;
};

template <typename TO, typename META_DATA>
TO const* unerase_cast(base<META_DATA> const& holded) {
  return static_cast<TO const*>(holded.value());
}
template <typename TO, typename META_DATA>
TO* unerase_cast(base<META_DATA>& holded) {
  return static_cast<TO*>(holded.value());
}

}  // namespace virtual_void::erased::data