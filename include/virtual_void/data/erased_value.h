#pragma once

#include <utility>

namespace virtual_void::data {

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

template <typename BASE>
class erased_value {
 public:
  erased_value() = default;
  template <typename DATA>
  erased_value(DATA* ptr)
    requires std::is_convertible_v<DATA*, BASE*>
      : ptr_(ptr), v_table_(&value_v_table_of<std::decay_t<DATA>>) {}
  erased_value(erased_value const& rhs)
      : ptr_(static_cast<BASE*>(rhs.v_table_->copy(rhs.ptr_))),
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
  auto& operator*() const
    requires(!std::same_as<void, BASE>)
  {
    return *ptr_;
  }
  BASE* operator->() const { return ptr_; }
  BASE* get() const { return ptr_; }
  explicit operator bool() const { return ptr_ != nullptr; }

  friend void swap(erased_value& lhs, erased_value& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(lhs.v_table_, rhs.v_table_);
  }

 private:
  BASE* ptr_ = nullptr;
  const value_v_table* v_table_ = nullptr;
};

template <typename BASE, typename T, typename... ARGS>
auto make_erased_value(ARGS&&... args)
  requires std::is_convertible_v<T*, BASE*>
{
  return erased_value<BASE>(new T(std::forward<ARGS>(args)...));
}

}  // namespace virtual_void::data