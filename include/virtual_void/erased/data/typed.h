#pragma once

#include "../../forward.h"

namespace virtual_void::erased {

#ifdef _DEBUG
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL virtual
#else
#define DATA_ALIGNED_DESRTUCTOR_VIRTUAL
#endif  // DEBUG

struct empty_meta_data {
  template <typename T>
  empty_meta_data(std::in_place_type_t<T>) {}
  type_info_ptr type_info() const { return {}; }
  void* data();
  void const* data() const;
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~empty_meta_data() = default;
};

template <typename T, typename META_DATA = empty_meta_data>
struct typed_data : META_DATA {
  using meta_data_t = META_DATA;
  T the_data_;
  template <typename... ARGS>
  typed_data(std::in_place_t in_place, ARGS&&... args)
      : META_DATA(std::in_place_type<T>),
        the_data_(std::forward<ARGS>(args)...) {}
};

void* empty_meta_data::data() {
  return &static_cast<typed_data<int, empty_meta_data>*>(this)->the_data_;
};
void const* empty_meta_data::data() const {
  return &static_cast<typed_data<int, empty_meta_data> const*>(this)->the_data_;
};

template <typename META_DATA>
struct ritch_meta_data {
  META_DATA meta_data_;
  template <typename T>
  ritch_meta_data(std::in_place_type_t<T>) : meta_data_(std::in_place_type<T>) {}
  void* data() {
    return &static_cast<typed_data<int, ritch_meta_data<META_DATA>>*>(this)
                ->the_data_;
  }
  void const* data() const {
    return &static_cast<typed_data<int, ritch_meta_data<META_DATA>> const*>(this)
                ->the_data_;
  }
  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~ritch_meta_data() = default;
};

struct type_info_ptr_holder {
  std::type_info const* type_info_;
  template <typename T>
  type_info_ptr_holder(std::in_place_type_t<T>)
      : type_info_(&typeid(std::decay_t<T>)) {}
};

struct type_info_meta_data : ritch_meta_data<type_info_ptr_holder> {
  using ritch_meta_data::ritch_meta_data;
  type_info_ptr type_info() const {
    return ritch_meta_data<type_info_ptr_holder>::meta_data_.type_info_;
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