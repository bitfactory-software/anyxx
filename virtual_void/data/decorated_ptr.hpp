#pragma once

#include <virtual_void/virtual_void.hpp>

namespace virtual_void::data {

template <typename PTR, typename META>
struct decorated_ptr : META {
  PTR ptr_ = nullptr;

  decorated_ptr() = default;
  decorated_ptr(const decorated_ptr& other) : META(other), ptr_(other.ptr_) {}
  decorated_ptr(decorated_ptr&& rhs) noexcept
      : META(std::move(*(META*)&rhs)), ptr_(std::move(rhs.ptr_)) {}
  decorated_ptr& operator=(decorated_ptr const& rhs) noexcept {
    decorated_ptr new_{rhs};
    swap(*this, new_);
    return *this;
  }
  decorated_ptr& operator=(decorated_ptr&& rhs) noexcept {
    decorated_ptr destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }

  decorated_ptr(PTR ptr, const META& meta) : ptr_(std::move(ptr)), META(meta) {}
  // template <is_virtual_void RHS>
  // decorated_ptr(RHS const& rhs)
  //   requires(is_const_data<decorated_ptr> == is_const_data<RHS>)
  //     : META(*get_meta(rhs)), ptr_(rhs) {}

  friend void swap(decorated_ptr& lhs, decorated_ptr& rhs) noexcept {
    using namespace std;
    swap(lhs.ptr_, rhs.ptr_);
    swap(static_cast<META&>(lhs), static_cast<META&>(rhs));
  }
};

}  // namespace virtual_void::data