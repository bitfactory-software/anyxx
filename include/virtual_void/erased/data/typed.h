#pragma once

#include "../../forward.h"

namespace virtual_void::erased {

#ifdef _DEBUG
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL virtual
#else
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL
#endif  // DEBUG

struct no_meta {
  template <typename T>
  no_meta(std::in_place_type_t<T>) {}
  type_info_ptr type_info() const { return {}; }
  void* data();
  void const* data() const;
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~no_meta() = default;
};

template <typename T, typename META_DATA = no_meta>
struct typed_data : META_DATA {
  using meta_data_t = META_DATA;
  T the_data_;
  template <typename... ARGS>
  typed_data(std::in_place_t in_place, ARGS&&... args)
      : META_DATA(std::in_place_type<T>),
        the_data_(std::forward<ARGS>(args)...) {}
};

void* no_meta::data() {
  return &static_cast<typed_data<int, no_meta>*>(this)->the_data_;
};
void const* no_meta::data() const {
  return &static_cast<typed_data<int, no_meta> const*>(this)->the_data_;
};

template <typename META_DATA>
struct with_meta {
  META_DATA meta_data_;
  template <typename T>
  with_meta(std::in_place_type_t<T>) : meta_data_(std::in_place_type<T>) {}
  void* data() {
    return &static_cast<typed_data<int, with_meta<META_DATA>>*>(this)
                ->the_data_;
  }
  void const* data() const {
    return &static_cast<typed_data<int, with_meta<META_DATA>> const*>(this)
                ->the_data_;
  }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~with_meta() = default;
};

struct type_info_ptr_holder {
  std::type_info const* type_info_;
  template <typename T>
  type_info_ptr_holder(std::in_place_type_t<T>)
      : type_info_(&typeid(std::decay_t<T>)) {}
};

struct with_type_info : with_meta<type_info_ptr_holder> {
  using with_meta::with_meta;
  type_info_ptr type_info() const {
    return with_meta<type_info_ptr_holder>::meta_data_.type_info_;
  }
};

template <typename TO, typename DATA>
TO const* unerase_data_cast(DATA const& data) {
  return static_cast<TO const*>(data.data());
}
template <typename TO, typename DATA>
TO* unerase_data_cast(DATA& data) {
  return static_cast<TO*>(data.data());
}

}