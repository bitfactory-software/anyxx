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
  template <typename T>
  base(std::in_place_type_t<T>) : META_DATA(std::in_place_type<T>) {}
  void* data();
  void const* data() const;
};

template <typename T, typename META_DATA = has_no_meta>
struct typed : base<META_DATA> {
  using meta_data_t = META_DATA;
  using base_t = base<META_DATA>;
  T the_data_;
  template <typename... ARGS>
  typed(std::in_place_t in_place, ARGS&&... args)
      : base_t(std::in_place_type<T>),
        the_data_(std::forward<ARGS>(args)...) {}
};

template <typename META_DATA>
void* base<META_DATA>::data() {
  return &static_cast<typed<int, META_DATA>*>(this)->the_data_;
};
template <typename META_DATA>
void const* base<META_DATA>::data() const {
  return &static_cast<typed<int, META_DATA> const*>(this)->the_data_;
};


using with_no_meta = base<has_no_meta>;
using with_type_info = base<has_type_info>;

template <typename TO, typename DATA>
TO const* unerase_cast(DATA const& data) {
  return static_cast<TO const*>(data.data());
}
template <typename TO, typename DATA>
TO* unerase_cast(DATA& data) {
  return static_cast<TO*>(data.data());
}

}  // namespace virtual_void::erased::data