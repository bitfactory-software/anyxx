#pragma once

#include "../../forward.h"

namespace virtual_void::data {

#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../forward.h"

namespace virtual_void::data {

template <typename VOID, typename META, typename PTR>
struct pointer_eraser : META {
  using void_t = VOID;
  using meta_t = META;
  static constexpr bool is_const = is_const_void<VOID>::value;

  pointer_eraser() = default;
  pointer_eraser(const pointer_eraser&) = default;
  pointer_eraser(pointer_eraser&) = default;
  template <typename T>
  pointer_eraser(T& v)
    requires(!std::derived_from<T, pointer_eraser> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void>)
      : META(std::in_place_type<T>), ptr_(&v) {}
  template <typename T>
  pointer_eraser(const T& v)
    requires(!std::derived_from<T, pointer_eraser> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void> &&
             is_const)
      : META(std::in_place_type<T>), ptr_(&v) {}
  pointer_eraser(void_t v, const META& meta) : ptr_(v), META(meta) {}

  pointer_eraser(pointer_eraser&& rhs) noexcept { swap(*this, rhs); }
  pointer_eraser& operator=(pointer_eraser&& rhs) noexcept {
    pointer_eraser destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend void swap(pointer_eraser& lhs, pointer_eraser& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(static_cast<META&>(lhs), static_cast<META&>(rhs));
  }

  VOID ptr_ = nullptr;
  VOID value() const { return ptr_; }
};

template <typename META, typename PTR>
using const_pointer_eraser = pointer_eraser<void const*, META, PTR>;
template <typename META, typename PTR>
using mutable_pointer_eraser = pointer_eraser<void*, META, PTR>;


}  // namespace virtual_void::data
}  // namespace virtual_void::data