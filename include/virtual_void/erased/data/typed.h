#pragma once

#include "../../forward.h"

#include "has_no_meta.h"
#include "has_type_info.h"

namespace virtual_void::erased::data {

#ifdef _DEBUG
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL virtual
#else
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL
#endif  // DEBUG

template <typename META_DATA = has_no_meta>
struct base : META_DATA {
  template <typename V>
  base(std::in_place_type_t<V>) : META_DATA(std::in_place_type<V>) {}
  void* value();
  void const* value() const;
};

template <typename V, typename META_DATA = has_no_meta>
struct typed : base<META_DATA> {
  using meta_data_t = META_DATA;
  using base_t = base<META_DATA>;
  using value_t = V;
  value_t value_;
  template <typename... ARGS>
  typed(std::in_place_t in_place, ARGS&&... args)
      : base_t(std::in_place_type<value_t>),
        value_(std::forward<ARGS>(args)...) {}
};

template <typename META_DATA>
void* base<META_DATA>::value() {
  return &static_cast<typed<int, META_DATA>*>(this)->value_;
};
template <typename META_DATA>
void const* base<META_DATA>::value() const {
  return &static_cast<typed<int, META_DATA> const*>(this)->value_;
};

using with_no_meta = base<has_no_meta>;
using with_type_info = base<has_type_info>;

template <typename TO, typename BASE>
TO const* unerase_cast(BASE const& data) {
  return static_cast<TO const*>(data.value());
}
template <typename TO, typename BASE>
TO* unerase_cast(BASE& data) {
  return static_cast<TO*>(data.value());
}

}  // namespace virtual_void::erased::data