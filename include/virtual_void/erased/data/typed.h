#pragma once

#include "../../forward.h"

namespace virtual_void::erased::data {

#ifdef _DEBUG
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL virtual
#else
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL
#endif  // DEBUG

struct has_no_meta {
  template <typename T>
  has_no_meta(std::in_place_type_t<T>) {}
  type_info_ptr type_info() const { return {}; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_no_meta() = default;
};

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

template <typename META_DATA>
struct with_meta {
  META_DATA meta_data_;
  template <typename T>
  with_meta(std::in_place_type_t<T>) : meta_data_(std::in_place_type<T>) {}
  void* data() {
    return &static_cast<typed<int, with_meta<META_DATA>>*>(this)->the_data_;
  }
  void const* data() const {
    return &static_cast<typed<int, with_meta<META_DATA>> const*>(this)
                ->the_data_;
  }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~with_meta() = default;
};

struct has_type_info {
  std::type_info const* type_info_;
  template <typename T>
  has_type_info(std::in_place_type_t<T>)
      : type_info_(&typeid(std::decay_t<T>)) {}
  type_info_ptr type_info() const { return type_info_; }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_type_info() = default;
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