#pragma once

#include "../virtual_void.h"

namespace virtual_void::data {

template <typename VOID, typename META>
struct observer_ptr : META {
  using void_t = VOID;
  using meta_t = META;
  static constexpr bool is_const = is_const_void1<VOID>;

  observer_ptr() = default;
  observer_ptr(const observer_ptr&) = default;
  observer_ptr(observer_ptr&) = default;
  observer_ptr(observer_ptr&& rhs) noexcept { swap(*this, rhs); }
  observer_ptr& operator=(observer_ptr&& rhs) noexcept {
    observer_ptr destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }

  observer_ptr(void_t v, const META& meta) : ptr_(v), META(meta) {}
  template <is_virtual_void RHS>
  observer_ptr(RHS const& rhs)
    requires(is_const_data<observer_ptr> == is_const_data<RHS>)
      : META(*get_meta(rhs)), ptr_(get_data(rhs)) {}

  friend void swap(observer_ptr& lhs, observer_ptr& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(static_cast<META&>(lhs), static_cast<META&>(rhs));
  }

  VOID ptr_ = nullptr;
  VOID value() const { return ptr_; }
};

template <typename META>
using const_observer_ptr = observer_ptr<void const*, META>;
template <typename META>
using mutable_observer_ptr = observer_ptr<void*, META>;

template <typename TO, typename META_DATA>
TO const* unerase_cast(observer_ptr<void const*, META_DATA> const& observed) {
  return static_cast<TO const*>(observed.value());
}
template <typename TO, typename META_DATA>
TO* unerase_cast(observer_ptr<void*, META_DATA>& observed) {
  return static_cast<TO*>(observed.value());
}

}  // namespace virtual_void::data