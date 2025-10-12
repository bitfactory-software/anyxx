#pragma once

#include <utility>

namespace virtual_void {

struct value_v_table {
  using destroy_fn = void(void*) noexcept;
  using copy_fn = void*(const void*);
  template <class DATA>
  constexpr value_v_table(std::in_place_type_t<DATA>)
      : destroy(&destroy_impl<DATA>), copy(&copy_impl<DATA>) {}
  template <class DATA>
  static void destroy_impl(void* target) noexcept {
    ::delete static_cast<DATA*>(target);
  }
  template <class DATA>
  static void* copy_impl(void const* source) {
    return ::new DATA(*static_cast<DATA const*>(source));
  }
  destroy_fn* destroy;
  copy_fn* copy;
};

template <class DATA>
constexpr value_v_table value_v_table_of =
    value_v_table(std::in_place_type<DATA>);

class erased_value {
 public:
  erased_value() = default;
  template <typename DATA>
  erased_value(DATA* ptr)
      : ptr_(ptr), v_table_(&value_v_table_of<std::decay_t<DATA>>) {}
  erased_value(erased_value const& rhs)
      : ptr_(rhs.ptr_ ? rhs.v_table_->copy(rhs.ptr_) : nullptr),
        v_table_(rhs.v_table_) {}
  erased_value& operator=(const erased_value& rhs) {
    erased_value clone{rhs};
    swap(*this, clone);
    return *this;
  }
  erased_value(erased_value&& rhs) noexcept { swap(*this, rhs); }
  erased_value& operator=(erased_value&& rhs) noexcept {
    erased_value destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  ~erased_value() {
    if (v_table_) v_table_->destroy(ptr_);
  }
  void* get() const { return ptr_; }
  explicit operator bool() const { return ptr_ != nullptr; }

  friend void swap(erased_value& lhs, erased_value& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(lhs.v_table_, rhs.v_table_);
  }

 private:
  void* ptr_ = nullptr;
  const value_v_table* v_table_ = nullptr;
};

template <typename T, typename... ARGS>
auto make_value(ARGS&&... args)
{
  return erased_value(new T(std::forward<ARGS>(args)...));
}

template <typename T, typename... ARGS>
auto make_void_value(ARGS&&... args) {
  return make_value<T>(std::forward<ARGS>(args)...);
}

template <typename T>
auto make_void_value(T&& v) {
  return make_value<T>(std::forward<T>(v));
}

template <typename U>
U& unsave_unerase_cast(erased_value& v) {
  return *static_cast<U*>(v.get());
}
template <typename U>
U const& unsave_unerase_cast(erased_value const& v) {
  return *static_cast<U const*>(v.get());
}


}  // namespace virtual_void