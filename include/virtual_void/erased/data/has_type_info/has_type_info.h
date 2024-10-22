#pragma once

#include "../holder.h"

namespace virtual_void::erased::data {

struct has_type_info {
  std::type_info const* type_info_ = nullptr;

  const has_type_info* meta() const { return this; }

  template <typename T>
  has_type_info(std::in_place_type_t<T>)
      : type_info_(&typeid(std::decay_t<T>)) {}

  has_type_info() noexcept = default;
  has_type_info(const has_type_info&) = default;
  has_type_info(has_type_info&) = default;
  has_type_info(has_type_info&& rhs) noexcept { swap(*this, rhs); }
  has_type_info& operator=(has_type_info&& rhs) noexcept {
    has_type_info destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend void swap(has_type_info& lhs, has_type_info& rhs) noexcept {
    using namespace std;
    swap(lhs.type_info_, rhs.type_info_);
  }

  type_info_ptr type_info() const { return type_info_; }

  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~has_type_info() = default;
};

using with_type_info = base<has_type_info>;

}  // namespace virtual_void::erased::data