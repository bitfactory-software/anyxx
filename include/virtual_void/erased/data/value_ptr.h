#pragma once

#include "typed.h"

namespace virtual_void::erased::data {

template <typename META_DATA>
struct value_v_table {
  using destroy_fn = void(META_DATA*) noexcept;
  using copy_fn = META_DATA*(const META_DATA*);
  template <class T>
  constexpr value_v_table(std::in_place_type_t<T>)
      : destroy(&destroy_impl<T>), copy(&copy_impl<T>) {}
  template <class T>
  static void destroy_impl(META_DATA* target) noexcept {
    ::delete static_cast<T*>(target);
  }
  template <class T>
  static META_DATA* copy_impl(META_DATA const* source) {
    return ::new T(*static_cast<T const*>(source));
  }
  destroy_fn* destroy;
  copy_fn* copy;
};

template <class T>
constexpr value_v_table<typename T::meta_data_t> value_v_table_of =
    value_v_table<typename T::meta_data_t>(std::in_place_type<T>);

template <typename META_DATA>
class value_ptr {
 public:
  template <typename DATA>
  value_ptr(DATA* v) : ptr_(v), v_table_(&value_v_table_of<std::decay_t<DATA>>) {}
  value_ptr(value_ptr const& rhs)
      : ptr_(rhs.v_table_->copy(rhs.ptr_)), v_table_(rhs.v_table_) {}
  ~value_ptr() { v_table_->destroy(ptr_); }
  META_DATA* value() { return ptr_; }
  META_DATA const* value() const { return ptr_; }
  META_DATA& operator*() { return *value(); }
  const META_DATA& operator*() const { return *value(); }
  META_DATA* operator->() { return value(); }
  const META_DATA* operator->() const { return value(); }

 private:
  META_DATA* ptr_;
  const value_v_table<META_DATA>* v_table_;
};

template <typename T, typename... ARGS>
auto make_value(ARGS&&... args) {
  using meta_data_t = T::meta_data_t;
  auto deleter = +[](meta_data_t* meta) { delete static_cast<T*>(meta); };
  return value_ptr<meta_data_t>(
      new T(std::in_place, std::forward<ARGS>(args)...));
}

}  // namespace virtual_void::erased::data