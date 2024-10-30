#pragma once

#include "../../forward.h"

namespace virtual_void::data::has_type_info {

struct meta {
  std::type_info const* type_info_ = nullptr;

  const auto* get_meta() const { return this; }

  template <typename T>
  meta(std::in_place_type_t<T>) : meta(typeid(std::decay_t<T>)) {}
  meta(std::type_info const& type_info) : type_info_(&type_info) {}

  meta() noexcept = default;
  meta(const meta&) = default;
  meta(meta&) = default;
  meta(meta&& rhs) noexcept { swap(*this, rhs); }
  meta& operator=(meta&& rhs) noexcept {
    meta destroy_this{};
    swap(*this, destroy_this);
    swap(*this, rhs);
    return *this;
  }
  friend void swap(meta& lhs, meta& rhs) noexcept {
    using namespace std;
    swap(lhs.type_info_, rhs.type_info_);
  }

  type_info_ptr type_info() const { return type_info_; }

  DATA_ALIGNED_DESRTUCTOR_VIRTUAL ~meta() = default;
};

}  // namespace virtual_void::data::has_type_info