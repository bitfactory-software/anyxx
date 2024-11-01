#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../forward.h"

namespace virtual_void::data {

template <typename META, typename PTR>
struct decorated_pointer : META {
  using meta_t = META;
 
  decorated_pointer() = default;
  decorated_pointer(const decorated_pointer&) = default;
  decorated_pointer(decorated_pointer&) = default;
  template <typename T>
  decorated_pointer(T& v)
    requires(!std::derived_from<T, decorated_pointer> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void>)
      : META(std::in_place_type<T>), ptr_(&v) {}
  template <typename T>
  decorated_pointer(const T& v)
    requires(!std::derived_from<T, decorated_pointer> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void> &&
             is_const)
      : META(std::in_place_type<T>), ptr_(&v) {}
  decorated_pointer(void_t v, const META& meta) : ptr_(v), META(meta) {}

  decorated_pointer(decorated_pointer&& rhs) noexcept { swap(*this, rhs); }
  decorated_pointer& operator=(decorated_pointer&& rhs) noexcept {
    decorated_pointer destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend void swap(decorated_pointer& lhs, decorated_pointer& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(static_cast<META&>(lhs), static_cast<META&>(rhs));
  }

  PTR ptr_ = nullptr;
};

}  // namespace virtual_void::data