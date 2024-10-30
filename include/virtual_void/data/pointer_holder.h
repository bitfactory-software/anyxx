#pragma once

#include "../../forward.h"

namespace virtual_void::data {

template <typename PTR, typename VOID, typename META>
struct pointer_holder : META {
  using void_t = VOID;
  using meta_t = META;
  static constexpr bool is_const = is_const_void<VOID>;

  pointer_holder() = default;
  pointer_holder(const pointer_holder&) = default;
  pointer_holder(pointer_holder&) = default;
  template <typename T>
  pointer_holder(PTR ptr)
    requires(!std::derived_from<T, pointer_holder> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void> &&
             !is_const)
      : META(std::in_place_type<typename T::value_type>), ptr_(std::move(ptr)) {}
  pointer_holder(pointer_holder&& rhs) noexcept { swap(*this, rhs); }
  pointer_holder& operator=(pointer_holder&& rhs) noexcept {
    pointer_holder destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend void swap(pointer_holder& lhs, pointer_holder& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(static_cast<META&>(lhs), static_cast<META&>(rhs));
  }

  VOID value() const { return ptr_; }

 private:
  PTR ptr_ = nullptr;
};

}  // namespace virtual_void::data