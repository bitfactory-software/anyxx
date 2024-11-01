#pragma once

#include <stdexcept>
#include <type_traits>

#include "../../forward.h"

namespace virtual_void::data {

template <typename META, typename PTR>
struct decorated_ptr : META {
  using meta_t = META;
 
  decorated_ptr() = default;
  decorated_ptr(const decorated_ptr&) = default;
  decorated_ptr(decorated_ptr&) = default;
  template <typename T>
  decorated_ptr(T& v)
    requires(!std::derived_from<T, decorated_ptr> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void>)
      : META(std::in_place_type<T>), ptr_(&v) {}
  template <typename T>
  decorated_ptr(const T& v)
    requires(!std::derived_from<T, decorated_ptr> &&
             !std::same_as<std::decay_t<std::remove_pointer_t<T>>, void> &&
             is_const)
      : META(std::in_place_type<T>), ptr_(&v) {}
  decorated_ptr(void_t v, const META& meta) : ptr_(v), META(meta) {}

  decorated_ptr(decorated_ptr&& rhs) noexcept { swap(*this, rhs); }
  decorated_ptr& operator=(decorated_ptr&& rhs) noexcept {
    decorated_ptr destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend void swap(decorated_ptr& lhs, decorated_ptr& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(static_cast<META&>(lhs), static_cast<META&>(rhs));
  }

  PTR ptr_ = nullptr;
};

}  // namespace virtual_void::data