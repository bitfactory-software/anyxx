#pragma once

#include "typed.h"

namespace virtual_void::erased::data {

template <typename BASE>
struct value_v_table {
  using destroy_fn = void(BASE*) noexcept;
  using copy_fn = BASE*(const BASE*);
  template <class T>
  constexpr value_v_table(std::in_place_type_t<T>)
      : destroy(&destroy_impl<T>), copy(&copy_impl<T>) {}
  template <class T>
  static void destroy_impl(BASE* target) noexcept {
    ::delete static_cast<T*>(target);
  }
  template <class T>
  static BASE* copy_impl(BASE const* source) {
    return ::new T(*static_cast<T const*>(source));
  }
  destroy_fn* destroy;
  copy_fn* copy;
};

template <class T>
constexpr value_v_table<typename T::base_t> value_v_table_of =
    value_v_table<typename T::base_t>(std::in_place_type<T>);

template <typename BASE>
class value_ptr {
 public:
  value_ptr() = default;
  template <typename DATA>
  value_ptr(DATA* v)
      : ptr_(v), v_table_(&value_v_table_of<std::decay_t<DATA>>) {}
  value_ptr(value_ptr const& rhs)
      : ptr_(rhs.v_table_->copy(rhs.ptr_)), v_table_(rhs.v_table_) {}
  value_ptr& operator=(const value_ptr& rhs) {
    value_ptr clone{rhs};
    swap(*this, clone);
    return *this;
  }
  value_ptr(value_ptr&& rhs) {
    swap(*this, rhs);
  }
  value_ptr& operator=(value_ptr&& rhs) {
    swap(*this, rhs);
    return *this;
  }
  ~value_ptr() {
    if (v_table_) v_table_->destroy(ptr_);
  }
  BASE* value() const { return ptr_; }
  BASE& operator*() const { return *value(); }
  BASE* operator->() const{ return value(); }
  explicit operator bool() const { return value() != nullptr; }

  friend void swap(value_ptr& lhs, value_ptr& rhs) {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(lhs.v_table_, rhs.v_table_);
  }

 private:
  BASE* ptr_ = nullptr;
  const value_v_table<BASE>* v_table_ = nullptr;
};

template <typename T, typename... ARGS>
auto make_value(ARGS&&... args) {
  using base_t = T::base_t;
  auto deleter = +[](base_t* meta) { delete static_cast<T*>(meta); };
  return value_ptr<base_t>(new T(std::in_place, std::forward<ARGS>(args)...));
}

}  // namespace virtual_void::erased::data