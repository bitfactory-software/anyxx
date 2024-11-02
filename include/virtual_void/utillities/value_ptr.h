#pragma once

namespace virtual_void {

template <typename BASE>
struct value_v_table {
  using destroy_fn = void(BASE*) noexcept;
  using copy_fn = BASE*(const BASE*);
  template <class DATA>
  constexpr value_v_table(std::in_place_type_t<DATA>)
    requires std::derived_from<DATA, BASE>
      : destroy(&destroy_impl<DATA>), copy(&copy_impl<DATA>) {}
  template <class DATA>
  static void destroy_impl(BASE* target) noexcept
    requires std::derived_from<DATA, BASE>
  {
    ::delete static_cast<DATA*>(target);
  }
  template <class DATA>
  static BASE* copy_impl(BASE const* source) {
    return ::new DATA(*static_cast<DATA const*>(source));
  }
  destroy_fn* destroy;
  copy_fn* copy;
};

template <class DATA, typename BASE>
constexpr value_v_table<typename BASE> value_v_table_of =
    value_v_table<BASE>(std::in_place_type<DATA>);

template <typename BASE>
class value_ptr {
 public:
  value_ptr() = default;
  template <typename DATA>
  value_ptr(DATA* ptr)
    requires std::is_convertible_v<DATA*, BASE*>
      : ptr_(ptr), v_table_(&value_v_table_of<std::decay_t<DATA>, BASE>) {}
  value_ptr(value_ptr const& rhs)
      : ptr_(rhs.v_table_->copy(rhs.ptr_)), v_table_(rhs.v_table_) {}
  value_ptr& operator=(const value_ptr& rhs) {
    value_ptr clone{rhs};
    swap(*this, clone);
    return *this;
  }
  value_ptr(value_ptr&& rhs) noexcept { swap(*this, rhs); }
  value_ptr& operator=(value_ptr&& rhs) noexcept {
    value_ptr destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  ~value_ptr() {
    if (v_table_) v_table_->destroy(ptr_);
  }
  auto operator*() const { return *ptr_; }
  BASE* operator->() const { return ptr_; }
  BASE* get() const { return ptr_; }
  explicit operator bool() const { return ptr_ != nullptr; }

  friend void swap(value_ptr& lhs, value_ptr& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(lhs.v_table_, rhs.v_table_);
  }

 private:
  BASE* ptr_ = nullptr;
  const value_v_table<BASE>* v_table_ = nullptr;
};

template <typename BASE, typename T, typename... ARGS>
auto make_value(ARGS&&... args)
  requires std::is_convertible_v<T*, BASE*>
{
  return value_ptr<BASE>(new T(std::forward<ARGS>(args)...));
}

}  // namespace virtual_void