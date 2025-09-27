#pragma once

#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

template <typename PTR>
struct decorated_ptr {
  PTR ptr_ = nullptr;

  friend void swap(decorated_ptr& lhs, decorated_ptr& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
  }

  explicit operator bool() const { return static_cast<bool>(ptr_); }
};

}  // namespace virtual_void::data